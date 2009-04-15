/** This is a portable wrapper over serial port communication.
  * MIT Licence. */
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <iostream>

#ifndef START_ERROR_BUFFER
#define START_ERROR_BUFFER 50
#endif

class SerialPort
{
public:
  
  SerialPort ()
  {
    output_ = &std::cout;
    mIsOpen = false;
    is_ok_   = true;
  }
  
  virtual ~SerialPort () 
  {
    if (mIsOpen) close(mFd);
    mIsOpen = false;
  }
  
  void set_output(std::ostream& pOutput)
  { output_ = &pOutput; }

  ///** Write 'size' chars from the buffer. */
  //int write(const char * buffer, int size)
  //{
  //  int n;
  //  n = write(mFd, buffer, size);
  //  if (n < 0) error(buf("write %i", size));
  //  return n;
  //}
  
  /** Open port. Options are:
    * pBauds          : an int (9600)
    * pCharSize       : an int (8)
    * pParityChecking : 'N' = no, 'E' = even, 'O' = odd, 'S' = space
    * pBlock          : block calls for input (wait for data)
    * pHardControl    : hardware flow control
    * pSoftControl    : software flow control
    * pRaw            : raw data (set to false if you want buffered lines)
  */
  bool init(const std::string &portName, int pBauds, int pCharSize, char pParityChecking, bool pBlock, bool pHardControl, bool pSoftControl, bool pRaw)
  {
    int opt;
    struct termios options;
    
    if (mIsOpen) close(mFd);
    mIsOpen = false;
    
    mPortName = portName;
    
    mFd = open(mPortName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (mFd == -1) {
      error(mPortName.c_str());
      mIsOpen = false;
      return false;
    } else
      mIsOpen = true;
        
        
    // Block until serial port has the characters during read.   BLOCK
    if (pBlock) 
      fcntl(mFd, F_SETFL, 0);
    else
      fcntl(mFd, F_SETFL, FNDELAY);
      
    // get options
    if (tcgetattr(mFd, &options) == -1) {
      error("tcgetattr");
      close(mFd);
      mIsOpen = false;
      return false;
    }
    
    // set baud rate                                          BAUDS
    switch(pBauds) {
    case     50: opt = B50;     break;
    case     75: opt = B75;     break;
    case    110: opt = B110;    break;
    case    134: opt = B134;    break;
    case    150: opt = B150;    break;
    case    200: opt = B200;    break;
    case    300: opt = B300;    break;
    case    600: opt = B600;    break;
    case   1200: opt = B1200;   break;
    case   1800: opt = B1800;   break;
    case   2400: opt = B2400;   break;
    case   4800: opt = B4800;   break;
    case   9600: opt = B9600;   break;
    case  19200: opt = B19200;  break;
    case  38400: opt = B38400;  break;
#ifdef B57600
    case  57600: opt = B57600;  break;
#endif
#ifdef B76800
    case  76800: opt = B76800;  break;
#endif
#ifdef B115200
    case 115200: opt = B115200; break;
#endif
#ifdef B230400
    case 230400: opt = B230400; break;
#endif
    default: 
    *output_ << "Unknown baud rate '" << pBauds << "'.\n";
      is_ok_ = false;
      return false;
    }
    cfsetispeed(&options, opt);
    cfsetospeed(&options, opt);
    
    // default flags
    options.c_cflag |= (CLOCAL | CREAD);
    // set character size                                     CHARACTER SIZE
    switch(pCharSize) {
    case 5: opt = CS5; break;
    case 6: opt = CS6; break;
    case 7: opt = CS7; break;
    case 8: opt = CS8; break;
    default:
      *output_ << "Unknown character size '" << pCharSize << "'.\n";
      is_ok_ = false;
      return false;
    }
    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= opt;    /* Select 8 data bits */
    
    
    // set parity checking                                    PARITY CHECKING
    switch(pParityChecking) {
    case 'N':  
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS8;
      break;
    case 'E':
      options.c_cflag |= PARENB;
      options.c_cflag &= ~PARODD;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
      break;
    case 'O':
      options.c_cflag |= PARENB;
      options.c_cflag |= PARODD;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS7;
      break;
    case 'S':
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS8;
      break;
    default:
      *output_ << "Unknown parity(must be 'N','E','O' or 'S') '" << pParityChecking << "'.\n";
      close(mFd);
      mIsOpen = false;
      is_ok_   = false;
      return false;
    }
    
    // hardware flow control                                  HARDWARE FLOW CONTROL
    if (pHardControl)
#ifdef CNEW_RTSCTS
      options.c_cflag |= CNEW_RTSCTS;
    else
      options.c_cflag &= ~CNEW_RTSCTS;
#else
      options.c_cflag |= CRTSCTS;
    else
      options.c_cflag &= ~CRTSCTS;
#endif
    
    // hardware flow control                                  SOFTWARE FLOW CONTROL
    if (pSoftControl)
      options.c_iflag |= (IXON | IXOFF | IXANY);
    else
      options.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    // local mode settings                                    RAW / CANONICAL
    if (pRaw)
      options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    else
      options.c_lflag |= (ICANON | ECHO | ECHOE);
      
    // do not process output
    options.c_oflag &= ~OPOST;
    
    // local mode settings                                    TIMEOUT
    // used with blocking calls
    options.c_cc[VMIN]  = 1; // if '0', use VTIME for every character
    options.c_cc[VTIME] = 0; // if VMIN is not 0, time to wait for first char
    
    // set
    if (tcsetattr(mFd, TCSANOW, &options) == -1) {
      error("tcsetattr");
      close(mFd);
      mIsOpen = false;
      return false;
    }
    
    return true;
  }
  
  
  /** Read 1 char. */
  bool read_char(unsigned char * c)
  {
    if (!is_ok_ || !mIsOpen) return false;
    
    return read(mFd, c, 1) == 1;
  }
  
  bool read_char(int * pRes)
  {
    if (!is_ok_ || !mIsOpen) return false;
    unsigned char c;
    if (read(mFd, &c, 1) == 1) {
      *pRes = (int)c;
      return true;
    } else
      return false;
  }
  
  bool write_char(char c)
  {
    return write(mFd, &c, 1) == 1;
  }
  //int getchar()
  //{ return getc(mFd); }

private:

  /** Report last error. */
  void error(const char * msg)
  {
    *output_ << msg << ": " << strerror(errno) << std::endl;
  }
  
  bool mIsOpen;
  bool is_ok_;
  int mFd; /**< File descriptor. */
  std::ostream * output_;
  std::string mPortName;
};