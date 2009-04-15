#line 1 "src/core/text_command.rl"
#include "text_command.h"
#include "rubyk.h"

//#define DEBUG_PARSER

#ifdef DEBUG_PARSER
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif


#line 15 "src/core/text_command.cpp"
static const char _text_command_actions[] = {
	0, 1, 0, 1, 1, 1, 3, 1, 
	4, 1, 5, 1, 11, 1, 12, 1, 
	13, 1, 15, 1, 16, 1, 17, 2, 
	0, 1, 2, 2, 6, 2, 2, 7, 
	2, 2, 8, 2, 2, 9, 2, 2, 
	10, 2, 3, 12, 2, 3, 13, 2, 
	5, 1, 2, 11, 15, 2, 12, 15, 
	2, 13, 15, 2, 14, 15, 3, 2, 
	9, 15, 3, 2, 10, 15, 3, 3, 
	12, 15, 3, 3, 13, 15, 3, 3, 
	14, 15, 3, 5, 0, 1, 4, 3, 
	12, 2, 7
};

static const short _text_command_key_offsets[] = {
	0, 0, 8, 11, 12, 20, 22, 34, 
	37, 45, 47, 51, 54, 61, 63, 70, 
	74, 76, 82, 82, 90, 97, 99, 104, 
	109, 109, 111, 119, 121, 128, 142, 146, 
	147, 151, 163, 165, 176, 181, 185, 193, 
	201, 203, 207, 210, 217, 219, 226, 230, 
	232, 238, 238, 246, 253, 255, 260, 265, 
	265, 267, 275, 277, 284, 288, 300, 302, 
	313, 321, 323, 327, 330, 337, 339, 346, 
	347, 349, 355, 355, 363, 370, 372, 377, 
	382, 382, 384, 392, 394, 401, 403, 417, 
	422, 423, 431, 433, 437, 440, 447, 449, 
	456, 460, 462, 468, 468, 476, 483, 485, 
	490, 495, 495, 497, 505, 507, 514, 515, 
	523
};

static const char _text_command_trans_keys[] = {
	9, 10, 32, 35, 65, 90, 97, 122, 
	9, 10, 32, 10, 46, 95, 48, 57, 
	65, 90, 97, 122, 97, 122, 9, 10, 
	32, 35, 40, 95, 48, 57, 65, 90, 
	97, 122, 9, 32, 35, 34, 41, 43, 
	45, 48, 57, 97, 122, 34, 92, 9, 
	32, 41, 44, 9, 32, 44, 9, 32, 
	34, 43, 45, 48, 57, 48, 57, 9, 
	32, 41, 44, 46, 48, 57, 9, 10, 
	32, 35, 48, 57, 9, 32, 41, 44, 
	48, 57, 58, 95, 48, 57, 65, 90, 
	97, 122, 9, 32, 34, 43, 45, 48, 
	57, 34, 92, 9, 32, 41, 97, 122, 
	9, 32, 41, 97, 122, 48, 57, 9, 
	32, 41, 46, 48, 57, 97, 122, 48, 
	57, 9, 32, 41, 48, 57, 97, 122, 
	9, 10, 32, 40, 46, 47, 61, 95, 
	48, 57, 65, 90, 97, 122, 9, 32, 
	47, 61, 47, 9, 32, 97, 122, 9, 
	10, 32, 35, 46, 95, 48, 57, 65, 
	90, 97, 122, 97, 122, 9, 10, 32, 
	35, 95, 48, 57, 65, 90, 97, 122, 
	9, 32, 62, 65, 90, 9, 32, 65, 
	90, 40, 95, 48, 57, 65, 90, 97, 
	122, 34, 41, 43, 45, 48, 57, 97, 
	122, 34, 92, 9, 32, 41, 44, 9, 
	32, 44, 9, 32, 34, 43, 45, 48, 
	57, 48, 57, 9, 32, 41, 44, 46, 
	48, 57, 9, 10, 32, 35, 48, 57, 
	9, 32, 41, 44, 48, 57, 58, 95, 
	48, 57, 65, 90, 97, 122, 9, 32, 
	34, 43, 45, 48, 57, 34, 92, 9, 
	32, 41, 97, 122, 9, 32, 41, 97, 
	122, 48, 57, 9, 32, 41, 46, 48, 
	57, 97, 122, 48, 57, 9, 32, 41, 
	48, 57, 97, 122, 9, 32, 97, 122, 
	9, 10, 32, 35, 46, 95, 48, 57, 
	65, 90, 97, 122, 97, 122, 9, 10, 
	32, 35, 95, 48, 57, 65, 90, 97, 
	122, 34, 41, 43, 45, 48, 57, 97, 
	122, 34, 92, 9, 32, 41, 44, 9, 
	32, 44, 9, 32, 34, 43, 45, 48, 
	57, 48, 57, 9, 32, 41, 44, 46, 
	48, 57, 10, 48, 57, 9, 32, 41, 
	44, 48, 57, 58, 95, 48, 57, 65, 
	90, 97, 122, 9, 32, 34, 43, 45, 
	48, 57, 34, 92, 9, 32, 41, 97, 
	122, 9, 32, 41, 97, 122, 48, 57, 
	9, 32, 41, 46, 48, 57, 97, 122, 
	48, 57, 9, 32, 41, 48, 57, 97, 
	122, 97, 122, 9, 10, 32, 35, 40, 
	47, 61, 95, 48, 57, 65, 90, 97, 
	122, 9, 32, 35, 47, 61, 62, 34, 
	41, 43, 45, 48, 57, 97, 122, 34, 
	92, 9, 32, 41, 44, 9, 32, 44, 
	9, 32, 34, 43, 45, 48, 57, 48, 
	57, 9, 32, 41, 44, 46, 48, 57, 
	9, 10, 32, 35, 48, 57, 9, 32, 
	41, 44, 48, 57, 58, 95, 48, 57, 
	65, 90, 97, 122, 9, 32, 34, 43, 
	45, 48, 57, 34, 92, 9, 32, 41, 
	97, 122, 9, 32, 41, 97, 122, 48, 
	57, 9, 32, 41, 46, 48, 57, 97, 
	122, 48, 57, 9, 32, 41, 48, 57, 
	97, 122, 10, 9, 10, 32, 35, 65, 
	90, 97, 122, 0
};

static const char _text_command_single_lengths[] = {
	0, 4, 3, 1, 2, 0, 6, 3, 
	4, 2, 4, 3, 5, 0, 5, 4, 
	0, 4, 0, 2, 5, 2, 3, 3, 
	0, 0, 4, 0, 3, 8, 4, 1, 
	2, 6, 0, 5, 3, 2, 2, 4, 
	2, 4, 3, 5, 0, 5, 4, 0, 
	4, 0, 2, 5, 2, 3, 3, 0, 
	0, 4, 0, 3, 2, 6, 0, 5, 
	4, 2, 4, 3, 5, 0, 5, 1, 
	0, 4, 0, 2, 5, 2, 3, 3, 
	0, 0, 4, 0, 3, 0, 8, 5, 
	1, 4, 2, 4, 3, 5, 0, 5, 
	4, 0, 4, 0, 2, 5, 2, 3, 
	3, 0, 0, 4, 0, 3, 1, 4, 
	0
};

static const char _text_command_range_lengths[] = {
	0, 2, 0, 0, 3, 1, 3, 0, 
	2, 0, 0, 0, 1, 1, 1, 0, 
	1, 1, 0, 3, 1, 0, 1, 1, 
	0, 1, 2, 1, 2, 3, 0, 0, 
	1, 3, 1, 3, 1, 1, 3, 2, 
	0, 0, 0, 1, 1, 1, 0, 1, 
	1, 0, 3, 1, 0, 1, 1, 0, 
	1, 2, 1, 2, 1, 3, 1, 3, 
	2, 0, 0, 0, 1, 1, 1, 0, 
	1, 1, 0, 3, 1, 0, 1, 1, 
	0, 1, 2, 1, 2, 1, 3, 0, 
	0, 2, 0, 0, 0, 1, 1, 1, 
	0, 1, 1, 0, 3, 1, 0, 1, 
	1, 0, 1, 2, 1, 2, 0, 2, 
	0
};

static const short _text_command_index_offsets[] = {
	0, 0, 7, 11, 13, 19, 21, 31, 
	35, 42, 45, 50, 54, 61, 63, 70, 
	75, 77, 83, 84, 90, 97, 100, 105, 
	110, 111, 113, 120, 122, 128, 140, 145, 
	147, 151, 161, 163, 172, 177, 181, 187, 
	194, 197, 202, 206, 213, 215, 222, 227, 
	229, 235, 236, 242, 249, 252, 257, 262, 
	263, 265, 272, 274, 280, 284, 294, 296, 
	305, 312, 315, 320, 324, 331, 333, 340, 
	342, 344, 350, 351, 357, 364, 367, 372, 
	377, 378, 380, 387, 389, 395, 397, 409, 
	415, 417, 424, 427, 432, 436, 443, 445, 
	452, 457, 459, 465, 466, 472, 479, 482, 
	487, 492, 493, 495, 502, 504, 510, 512, 
	519
};

static const unsigned char _text_command_indicies[] = {
	1, 2, 1, 3, 4, 5, 0, 1, 
	2, 1, 0, 2, 3, 6, 4, 4, 
	4, 4, 0, 7, 0, 8, 9, 8, 
	10, 11, 7, 7, 7, 7, 0, 12, 
	12, 3, 0, 13, 14, 15, 15, 16, 
	17, 0, 19, 20, 18, 21, 21, 22, 
	23, 0, 24, 24, 25, 0, 25, 25, 
	13, 15, 15, 16, 0, 16, 0, 21, 
	21, 22, 23, 26, 16, 0, 27, 28, 
	27, 29, 0, 30, 0, 21, 21, 22, 
	23, 30, 0, 18, 31, 17, 17, 17, 
	17, 0, 31, 31, 32, 33, 33, 34, 
	0, 36, 37, 35, 38, 38, 22, 39, 
	0, 40, 40, 14, 17, 0, 35, 34, 
	0, 38, 38, 22, 41, 34, 39, 0, 
	42, 0, 38, 38, 22, 42, 39, 0, 
	43, 44, 43, 45, 46, 47, 48, 5, 
	5, 5, 5, 0, 49, 49, 50, 51, 
	0, 52, 0, 52, 52, 53, 0, 54, 
	55, 54, 56, 57, 53, 53, 53, 53, 
	0, 58, 0, 54, 55, 54, 56, 58, 
	58, 58, 58, 0, 59, 59, 60, 61, 
	0, 59, 59, 61, 0, 62, 61, 61, 
	61, 61, 0, 63, 64, 65, 65, 66, 
	67, 0, 69, 70, 68, 71, 71, 72, 
	73, 0, 74, 74, 75, 0, 75, 75, 
	63, 65, 65, 66, 0, 66, 0, 71, 
	71, 72, 73, 76, 66, 0, 77, 78, 
	77, 79, 0, 80, 0, 71, 71, 72, 
	73, 80, 0, 68, 81, 67, 67, 67, 
	67, 0, 81, 81, 82, 83, 83, 84, 
	0, 86, 87, 85, 88, 88, 72, 89, 
	0, 90, 90, 64, 67, 0, 85, 84, 
	0, 88, 88, 72, 91, 84, 89, 0, 
	92, 0, 88, 88, 72, 92, 89, 0, 
	60, 60, 93, 0, 94, 95, 94, 96, 
	97, 93, 93, 93, 93, 0, 98, 0, 
	94, 95, 94, 96, 98, 98, 98, 98, 
	0, 99, 100, 101, 101, 102, 103, 0, 
	105, 106, 104, 107, 107, 108, 109, 0, 
	110, 110, 111, 0, 111, 111, 99, 101, 
	101, 102, 0, 102, 0, 107, 107, 108, 
	109, 112, 102, 0, 113, 0, 114, 0, 
	107, 107, 108, 109, 114, 0, 104, 115, 
	103, 103, 103, 103, 0, 115, 115, 116, 
	117, 117, 118, 0, 120, 121, 119, 122, 
	122, 108, 123, 0, 124, 124, 100, 103, 
	0, 119, 118, 0, 122, 122, 108, 125, 
	118, 123, 0, 126, 0, 122, 122, 108, 
	126, 123, 0, 127, 0, 128, 129, 128, 
	130, 131, 132, 133, 127, 127, 127, 127, 
	0, 134, 134, 3, 50, 135, 0, 60, 
	0, 136, 137, 138, 138, 139, 140, 0, 
	142, 143, 141, 144, 144, 145, 146, 0, 
	147, 147, 148, 0, 148, 148, 136, 138, 
	138, 139, 0, 139, 0, 144, 144, 145, 
	146, 149, 139, 0, 150, 151, 150, 152, 
	0, 153, 0, 144, 144, 145, 146, 153, 
	0, 141, 154, 140, 140, 140, 140, 0, 
	154, 154, 155, 156, 156, 157, 0, 159, 
	160, 158, 161, 161, 145, 162, 0, 163, 
	163, 137, 140, 0, 158, 157, 0, 161, 
	161, 145, 164, 157, 162, 0, 165, 0, 
	161, 161, 145, 165, 162, 0, 167, 166, 
	1, 2, 1, 3, 4, 5, 0, 168, 
	0
};

static const char _text_command_trans_targs[] = {
	0, 2, 111, 3, 4, 29, 5, 6, 
	7, 111, 3, 8, 7, 9, 15, 13, 
	14, 19, 9, 10, 18, 11, 15, 12, 
	11, 12, 16, 7, 111, 3, 17, 20, 
	21, 25, 26, 21, 22, 24, 23, 19, 
	23, 27, 28, 30, 111, 64, 85, 31, 
	36, 30, 31, 36, 32, 33, 7, 111, 
	3, 34, 35, 37, 60, 38, 39, 40, 
	46, 44, 45, 50, 40, 41, 49, 42, 
	46, 43, 42, 43, 47, 7, 111, 3, 
	48, 51, 52, 56, 57, 52, 53, 55, 
	54, 50, 54, 58, 59, 61, 7, 111, 
	3, 62, 63, 65, 71, 69, 70, 75, 
	65, 66, 74, 67, 71, 68, 67, 68, 
	72, 111, 73, 76, 77, 81, 82, 77, 
	78, 80, 79, 75, 79, 83, 84, 86, 
	87, 111, 3, 89, 31, 88, 87, 88, 
	90, 96, 94, 95, 100, 90, 91, 99, 
	92, 96, 93, 92, 93, 97, 7, 111, 
	3, 98, 101, 102, 106, 107, 102, 103, 
	105, 104, 100, 104, 108, 109, 110, 112, 
	0
};

static const char _text_command_trans_actions[] = {
	19, 0, 17, 0, 1, 1, 7, 1, 
	44, 74, 44, 5, 0, 3, 0, 23, 
	23, 23, 23, 3, 23, 47, 9, 47, 
	3, 3, 23, 15, 56, 15, 23, 3, 
	3, 23, 23, 23, 3, 23, 47, 82, 
	3, 23, 23, 26, 78, 5, 26, 26, 
	26, 0, 0, 0, 0, 1, 38, 66, 
	38, 32, 1, 0, 0, 1, 7, 3, 
	0, 23, 23, 23, 23, 3, 23, 47, 
	9, 47, 3, 3, 23, 11, 50, 11, 
	23, 3, 3, 23, 23, 23, 3, 23, 
	47, 82, 3, 23, 23, 1, 35, 62, 
	35, 32, 1, 3, 0, 23, 23, 23, 
	23, 3, 23, 47, 9, 47, 3, 3, 
	23, 59, 23, 3, 3, 23, 23, 23, 
	3, 23, 47, 82, 3, 23, 23, 1, 
	86, 70, 41, 5, 29, 29, 0, 0, 
	3, 0, 23, 23, 23, 23, 3, 23, 
	47, 9, 47, 3, 3, 23, 13, 53, 
	13, 23, 3, 3, 23, 23, 23, 3, 
	23, 47, 82, 3, 23, 23, 0, 21, 
	0
};

static const char _text_command_eof_actions[] = {
	0, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 19, 19, 
	19, 19, 19, 19, 19, 19, 0, 0, 
	0
};

static const int text_command_start = 1;
static const int text_command_first_final = 111;

static const int text_command_en_eat_line = 110;
static const int text_command_en_main = 1;

#line 15 "src/core/text_command.rl"


void TextCommand::initialize() {
  int cs;
  
  planet_     = NULL;
  token_i_    = 0;
  silent_     = false;
  
  clear();
  
  
#line 328 "src/core/text_command.cpp"
	{
	cs = text_command_start;
	}
#line 27 "src/core/text_command.rl"
  current_state_ = cs;
}

void TextCommand::do_listen() {
  char buffer[1024];
  char * line = buffer;
  
  if (!silent_) *output_ << "Welcome to rubyk !\n\n";
    
  clear();

  while(thread_.run() && getline(&line,1023)) {
    parse(line);
    parse("\n");
    if (thread_.run()) saveline(line); // command was not a 'quit'
    freeline(line);
  }
}

void TextCommand::parse(const std::string &string) {
  const char *p  = string.data();     // data pointer
  const char *pe = p + string.size(); // past end
  const char *eof = NULL;             // FIXME: this should be set to 'pe' on the last string block...
  int cs = current_state_;            // restore machine state
  
  DEBUG(printf("parse:\"%s\"\n",string.c_str()));
  
  
#line 361 "src/core/text_command.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _text_command_trans_keys + _text_command_key_offsets[cs];
	_trans = _text_command_index_offsets[cs];

	_klen = _text_command_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _text_command_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _text_command_indicies[_trans];
	cs = _text_command_trans_targs[_trans];

	if ( _text_command_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _text_command_actions + _text_command_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 55 "src/core/text_command.rl"
	{
      if (token_i_ >= MAX_TOKEN_SIZE) {
        std::cerr << "Buffer overflow !" << std::endl;
        // stop parsing
        return;
      }
      DEBUG(printf("_%c_",(*p)));
      token_[token_i_] = (*p); /* append */
      token_i_++;     
    }
	break;
	case 1:
#line 66 "src/core/text_command.rl"
	{
      // FIXME: this is a temporary hack until we sub parse with Value...
      parameter_string_ += (*p);
    }
	break;
	case 2:
#line 71 "src/core/text_command.rl"
	{ set_from_token(var_);}
	break;
	case 3:
#line 73 "src/core/text_command.rl"
	{ set_from_token(method_);}
	break;
	case 4:
#line 75 "src/core/text_command.rl"
	{ set_from_token(class_);}
	break;
	case 5:
#line 77 "src/core/text_command.rl"
	{ set_from_token(value_);}
	break;
	case 6:
#line 79 "src/core/text_command.rl"
	{ from_node_     = var_; }
	break;
	case 7:
#line 81 "src/core/text_command.rl"
	{ from_port_ = var_; }
	break;
	case 8:
#line 83 "src/core/text_command.rl"
	{ to_port_   = var_; }
	break;
	case 9:
#line 89 "src/core/text_command.rl"
	{
      to_node_   = var_;
      create_link();
    }
	break;
	case 10:
#line 94 "src/core/text_command.rl"
	{
      to_node_   = var_;
      remove_link();
    }
	break;
	case 11:
#line 99 "src/core/text_command.rl"
	{ create_instance(); }
	break;
	case 12:
#line 101 "src/core/text_command.rl"
	{ execute_method(); }
	break;
	case 13:
#line 103 "src/core/text_command.rl"
	{ execute_class_method(); }
	break;
	case 14:
#line 105 "src/core/text_command.rl"
	{ execute_command(); }
	break;
	case 15:
#line 109 "src/core/text_command.rl"
	{
      clear();
    }
	break;
	case 16:
#line 113 "src/core/text_command.rl"
	{
      p--; // move back one char
      char error_buffer[10];
      snprintf(error_buffer, 9, "%s", p);
      *output_ << "Syntax error near '" << error_buffer << "'." << std::endl;
      clear();
      {cs = 110; goto _again;} // eat the rest of the line and continue parsing
    }
	break;
	case 17:
#line 122 "src/core/text_command.rl"
	{ {cs = 1; goto _again;} }
	break;
#line 534 "src/core/text_command.cpp"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _text_command_actions + _text_command_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 16:
#line 113 "src/core/text_command.rl"
	{
      p--; // move back one char
      char error_buffer[10];
      snprintf(error_buffer, 9, "%s", p);
      *output_ << "Syntax error near '" << error_buffer << "'." << std::endl;
      clear();
      {cs = 110; goto _again;} // eat the rest of the line and continue parsing
    }
	break;
#line 561 "src/core/text_command.cpp"
		}
	}
	}

	_out: {}
	}
#line 174 "src/core/text_command.rl"

//  printf("{%s}\n",p);
  current_state_ = cs;
}

void TextCommand::set_from_token(std::string& string) {
  token_[token_i_] = '\0';
  
  DEBUG(if (&string == &value_) std::cout << "[val " << token_ << "]" << std::endl);
  DEBUG(if (&string == &var_)   std::cout << "[var " << token_ << "]" << std::endl);
  DEBUG(if (&string == &class_) std::cout << "[cla " << token_ << "]" << std::endl);
  
  string = token_;
  token_i_ = 0;
}

void TextCommand::create_instance() {
  Value params(Json(parameter_string_.c_str()));
  names_to_urls();
  
  DEBUG(std::cout << "NEW "<< var_ << " = " << class_ << "(" << params << ")");
  Value res = planet_->new_object(var_, class_, params);
  
  if (res.is_string() && !silent_) {
    res = planet_->call("/.inspect", res);
    *output_ << res << std::endl;
  } else if (!res.is_string()) {
    *output_ << res << std::endl;
  } 
}


void TextCommand::create_link() {
  names_to_urls();
  
  DEBUG(std::cout << "LINK " << from_node_ << "." << from_port_ << "=>" << to_port_ << "." << to_node_ << std::endl);
  planet_->create_link(from_node_, from_port_, to_port_, to_node_);
}

void TextCommand::remove_link() {
  names_to_urls();
  
  DEBUG(std::cout << "UNLINK " << from_node_ << "." << from_port_ << "=>" << to_port_ << "." << to_node_ << std::endl);
  planet_->remove_link(from_node_, from_port_, to_port_, to_node_);
}

void TextCommand::execute_method() {
  Value res;
  // why doesn't this work ? Value params(Json(parameter_string_));
  Value params = Value(Json(parameter_string_));
  names_to_urls();
  
  DEBUG(std::cout << "METHOD " << var_ << "." << method_ << "(" << params << ")" << std::endl);
  
  if (method_ == "set") {
    // TODO: should 'set' live in normal tree space ?
    BaseObject *target = planet_->object_at(var_);
    if (target) {
      target->lock();
        res = target->set(params);
      target->unlock();
    } else {
      res = ErrorValue(NOT_FOUND_ERROR, var_);
    }
  } else {
    if (method_ == "b") method_ = "bang";
    var_.append("/").append(method_);
    res = planet_->call(var_, params);
  }
  
  if (!silent_) *output_ << res << std::endl;
}

void TextCommand::execute_class_method() {
  Value res;
  Value params = Value(Json(parameter_string_));
  
  DEBUG(std::cout << "CLASS_METHOD " << std::string(CLASS_ROOT).append("/").append(class_).append("/").append(method_) << "(" << params << ")" << std::endl);
  res = planet_->call(std::string(CLASS_ROOT).append("/").append(class_).append("/").append(method_), params);
  
  if (!silent_) *output_ << res << std::endl;
}

void TextCommand::execute_command() {
  Value res;
  Value params = Value(Json(parameter_string_));

  DEBUG(std::cout << "CMD " << method_ << "(" << params << ")" << std::endl);
  if (method_ == "set_lib_path") {
    res = planet_->call(std::string(CLASS_ROOT).append("/lib_path"), params);
  } else if (method_ == "quit" || method_ == "q") {
    // TODO: move 'quit' into osc space "/quit"
    planet_->quit();
  } else {  
    method_.insert(0, current_directory_);
    res = planet_->call(method_, params);
  }
  if (!silent_) *output_ << res << std::endl;
}

void TextCommand::clear() {
  token_i_    = 0;
  var_        = "";
  class_      = "";
  parameter_string_ = "";
  from_port_  = "";
  to_port_    = "";
}