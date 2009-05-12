#include "rubyk.h"

enum replay_modes_t {
  WaitingMode = 0, /**< No data to playback, recording not started. */
  PlaybackMode,   /**< Playback. */
  RecordingMode,  /**< Recording. */
};

class Replay : public Node
{
public:
  ~Replay()
  {
    if (mFileHandle) fclose(mFileHandle);
  }
  
  bool init(const Value &p)
  {
    mFileHandle = NULL;
    mState      = WaitingMode;
    mFilePath   = name_;
    mIsLoop     = true;
    mFilePath.append(".rec");
    
    mS.set(mPlaybackView);
    TRY(mData, set_sizes(0,0));
    
    return true;
  }
  
  bool set(const Value &p)
  {
    size_t size;
    if (p.get(&size, "vector")) TRY(mData, set_sizes(0,size));
    p.get(&mIsLoop, "loop");
    
    if (p.get(&mFilePath, "file", true)) {
      if (reload_data(true)) enter(PlaybackMode);
    }
    return true;
  }
  
  void bang(const Value &val)
  {
    int cmd;
    if (val.get(&cmd)) {
      // execute command
      if (cmd == (int)'r') {
        enter(RecordingMode);
      } else if (cmd == (int)'p') {
        enter(PlaybackMode);
      }
    } else if (mState == RecordingMode && val.get(&mLiveBuffer)) {
      // record
      // we set our view on this buffer because we need to print it (cannot print a const matrix since it can
      // alter the matrix by setting an error message)
      TRY_RET(mPlaybackView, set_view(*mLiveBuffer));
      record_matrix(mPlaybackView);
      send(mS);
    } else if (mState == PlaybackMode) {
      if (!mData.size()) send(gNilValue);
      if (mIndex >= mData.row_count()) {
        if (mIsLoop) mIndex = 0;
        else return;
      }
      TRY_RET(mPlaybackView, set_view(mData, mIndex, mIndex));
      send(mS);
      mIndex++;
    } else {
      // pass through
      send(sig);
    }
  }
  
  void play(const Value &p)
  { enter(PlaybackMode); }
  
  void record(const Value &p)
  { enter(RecordingMode); }
  
  void reload(const Value &p)
  { reload_data(); }
  
  virtual const Value inspect(const Value &val) 
  {
    bprint(mSpy, mSpySize, "%u / %ux%u", mIndex, mData.row_count(), mData.col_count());
  }
  
private:
  
  void enter(replay_modes_t pMode)
  {
    if (mFileHandle && pMode != RecordingMode) fclose(mFileHandle);
    
    switch(pMode) {
    case PlaybackMode:
      *output_ << name_ << ": starting playback (buffer " << mData.row_count() << "x" << mData.col_count() << ").\n";
      mState = PlaybackMode;
      break;
    case RecordingMode:
      if (mState == RecordingMode) return;
      TRY_RET(mData, set_sizes(0, mData.col_count()));
      mData.clear();
      *output_ << name_ << ": ready to record.\n";
      mState = RecordingMode;
      break;
    default:
      mState = WaitingMode;
    }
  }
  
  bool record_matrix(Matrix& pLiveView)
  {
    if (!mFileHandle) {
      // start recording
      if (!mData.col_count()) {// set vector size
        TRY(mData, set_sizes(0, mLiveBuffer->col_count()));
      }
      
      *output_ << name_ << ": recording started (vector size " << mData.col_count() << ").\n";
      // open file handle
      mFileHandle = fopen(mFilePath.c_str(), "wb");
      if (!mFileHandle) {
        *output_ << name_ << ": could not open '" << mFilePath << "' for writing.\n";
        return false;
      }
    }
    TRY(mData,     append(pLiveView.data, pLiveView.size()));
    TRY(pLiveView, to_file(mFileHandle, false));
    return true;
  }
  
  bool reload_data(bool accept_no_file = false)
  {
    TRY(mData, set_sizes(0, mData.col_count()));
    if (accept_no_file) {
      mData.from_file(mFilePath, "rb");
    } else {
      TRY(mData, from_file(mFilePath, "rb"));
    }
    mIndex = 0;
    return mData.row_count() != 0;
  }
  
  replay_modes_t mState;         /**< Playback, recording, waiting. */
  
  FILE * mFileHandle;            /**< File handle opened during recording. */
  
  std::string mFilePath;         /**< Where to read/write data. */
  Matrix mData;                  /**< In memory data from file to playback. */
  const Matrix * mLiveBuffer;    /**< Streaming data to be writtend to file. */
  CutMatrix mPlaybackView;       /**< View inside mData sent during playback. */
  size_t mIndex;                 /**< Current reading position in mData. */
  bool   mIsLoop;                /**< If true the values are read in loop (default = true). */
};


extern "C" void init(Planet &planet) {
  CLASS (Replay)
  OUTLET(Replay,stream)
  METHOD(Replay,play)
  METHOD(Replay,record)
  METHOD(Replay,reload)
}