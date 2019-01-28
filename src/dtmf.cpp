// -----------------------------------------------------------------
// dtmflib - a library to generate DTMF tones, encode/decode data.
// Copyright (C) 2019  Gabriele Bonacini
//
// This program is free software for no profit use; you can redistribute 
// it and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// A commercial license is also available for a lucrative use.
// -----------------------------------------------------------------

#include <dtmf.hpp>
#include <cmath>

namespace dtmfutil {

    using std::make_pair;
    using std::make_tuple;
    using std::get;
    using std::vector;
    using std::string;
    using std::to_string;
    using std::endl;
    using std::toupper;
    using std::ios;
    using std::hex;
    using std::setw;
    using std::setfill;
    using std::ostream;
    using std::ofstream;
    using std::istream;
    using std::istreambuf_iterator;
    using std::array;
    using std::out_of_range;

    istream& operator>> (istream& in, const Dtmf& dtmf){
        for( istreambuf_iterator<char> it(in.rdbuf()); it != istreambuf_iterator<char>(); ++it)
            dtmf.play(*it & 0xFF);

        return in;
    }

    ostream& operator<< (ostream& out, const DtmfToBinFile& dtf){
        try{
            uint8_t lower{ 0x0 },
                    upper{ 0x0 },
                    merge{ 0x0 };
            size_t  ch{ 1 };

            for( istreambuf_iterator<char> it(dtf.iFile.rdbuf());
                 it != istreambuf_iterator<char>();
                 ++it, ++ch){
                     if( (ch % 2) != 0){
                         upper  =  dtf.dtmfToBin.at(*it) << 4;
                     }else{
                         merge  =  0x0;
                         lower  =  dtf.dtmfToBin.at(*it);
                         merge  =  merge | lower | upper;
                         out    << merge;
                     }
            }

            if( (ch % 2) != 0)
                 throw string("Dtmf decoding: Invalid trailing character");

        }catch(out_of_range& err){
                 static_cast<void>(err);
                 throw string("Dtmf decoding: Invalid value.");
        }catch(...){
                 throw string("Dtmf decoding: Unexpected Error.");
        }

        return out;
    }


    ostream& operator<< (ostream& out, const BinFileToDtmf& ftd){
        for( istreambuf_iterator<char> it(ftd.iFile.rdbuf());
             it != istreambuf_iterator<char>();
             ++it){
                 unsigned char lower  { static_cast<unsigned char>(*it & 0b0000'1111 ) },
                               upper  { static_cast<unsigned char>((*it & 0b1111'0000 ) >> 4 ) };
                 out    << ftd.binToDtmf[upper];
                 out    << ftd.binToDtmf[lower];
        }

        return out;
    }

    DtmfToBinFile::DtmfToBinFile(const std::string& fileName)
       : iFile(fileName, ios::in | ios::binary),
         dtmfToBin { {'0', 0x0 }, {'1', 0x1 }, {'2', 0x2 }, {'3', 0x3 },
                     {'4', 0x4 }, {'5', 0x5 }, {'6', 0x6 }, {'7', 0x7 },
                     {'8', 0x8 }, {'9', 0x9 }, {'A', 0xA }, {'B', 0xB },
                     {'C', 0xC }, {'D', 0xD }, {'#', 0xE }, {'*', 0xF }  }
    {}

    DtmfToBinFile::~DtmfToBinFile(void){
           iFile.close();
    }

    bool DtmfToBinFile::saveTo(const string& outFile) const noexcept{
        bool ret{ true };
        try{
            oFile.open (outFile, ofstream::out);
            oFile << *this;
            oFile.close();
        }catch(...){
            ret  =  false;
        }

        return ret;
    }

    BinFileToDtmf::BinFileToDtmf(const std::string& fileName)
       : iFile(fileName, ios::in | ios::binary),
         binToDtmf { '0', '1', '2', '3', '4', '5', '6', '7',
                     '8', '9', 'A', 'B', 'C', 'D', '#', '*'  }
    {}

    BinFileToDtmf::~BinFileToDtmf(void){
           iFile.close();
    }

    bool BinFileToDtmf::saveTo(const string& outFile) const noexcept{
        bool ret{ true };
        try{
            oFile.open (outFile, ofstream::out);
            oFile << *this;
            oFile.close();
        }catch(...){
            ret  =  false;
        }

        return ret;
    }

    AlsaPlay::AlsaPlay(size_t sSize, string dev)
     : handle{ nullptr },
       frames{ 0 },
       device{ dev },
       sampleSize{ sSize }
    {
        int err{ 0 };
        if((err = snd_pcm_open(&handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            throw string("AlsaPlay::AlsaPlay : can't open pcm : ").append(snd_strerror(err));
        }

        if((err = snd_pcm_set_params(handle,
                            SND_PCM_FORMAT_U8,
                            SND_PCM_ACCESS_RW_INTERLEAVED,
                            1,
                            sampleSize,
                            1,
                            249999)) < 0) {
            throw string("AlsaPlay::AlsaPlay : can't set pcm : ").append(snd_strerror(err));
        }
    }

    AlsaPlay::~AlsaPlay(void){
        snd_pcm_drain(handle);
        snd_pcm_close(handle);
    }

    bool AlsaPlay::play(const RawSoundArray& arr, string& errmsg) const noexcept{
        frames = snd_pcm_writei(handle, arr.data(), arr.size());
        if (frames < 0){
            frames = snd_pcm_recover(handle, frames, 0);
            errmsg = string("snd_pcm_writei failed: ").append(snd_strerror(frames));
            return false;
        }
        if(static_cast<size_t>(frames) < arr.size()){
            errmsg = string("Short write (expected: ").append(to_string(arr.size()))\
                           .append(" wrote: ").append(to_string(frames));
            return false;
        }
        return true;
    }

    Dtmf::Dtmf(size_t sSize, size_t whiteFract, bool whsound)
     : sampleSize{ sSize },
       sampleNum{ static_cast<size_t>((0.25F/(1.0F/sampleSize))) },
       dtmfMap { make_pair('1', make_tuple(1209, 697, vector<unsigned char>())),
                 make_pair('2', make_tuple(1336, 697, vector<unsigned char>())),
                 make_pair('3', make_tuple(1477, 697, vector<unsigned char>())),
                 make_pair('4', make_tuple(1209, 770, vector<unsigned char>())),
                 make_pair('5', make_tuple(1336, 770, vector<unsigned char>())),
                 make_pair('6', make_tuple(1477, 770, vector<unsigned char>())),
                 make_pair('7', make_tuple(1209, 852, vector<unsigned char>())),
                 make_pair('8', make_tuple(1336, 852, vector<unsigned char>())),
                 make_pair('9', make_tuple(1477, 852, vector<unsigned char>())),
                 make_pair('0', make_tuple(1336, 941, vector<unsigned char>())),
                 make_pair('*', make_tuple(1209, 941, vector<unsigned char>())),
                 make_pair('#', make_tuple(1477, 941, vector<unsigned char>())),
                 make_pair('A', make_tuple(1633, 697, vector<unsigned char>())),
                 make_pair('B', make_tuple(1633, 770, vector<unsigned char>())),
                 make_pair('C', make_tuple(1633, 852, vector<unsigned char>())),
                 make_pair('D', make_tuple(1633, 941, vector<unsigned char>()))},
       aplay(sSize),
       white(sSize/whiteFract),
       whiteSound{whsound},
       errMsg{"No error"}
    {}

    float Dtmf::bgsine(short in) noexcept{
        static constexpr float coeff { 2 * M_PI * ( 1 / 65535.0F ) };
        return sinf( in * coeff );
    }

    void Dtmf::init(size_t tlen) noexcept{
       for(auto& el : dtmfMap){
           unsigned int ad1 { static_cast<unsigned int>((get<TONE_1>(el.second) << 16) / sampleSize) },
                        ad2 { static_cast<unsigned int>((get<TONE_2>(el.second) << 16) / sampleSize) };
           size_t       l   { (tlen * sampleSize) / 1000 };
           short        c1  { 0 },
                        c2  { 0 };

           for(size_t idx { 0 };  idx<l;  idx++, c1+= ad1, c2+= ad2 ) {
                float   y   {  (Dtmf::bgsine(c1) + Dtmf::bgsine(c2)) * 0.5F };
                (get<DATA>(el.second)).push_back(static_cast<unsigned char>((y + 1.0F) * 127.0F));
           }
       }
    }

    bool Dtmf::play(const string& str) const noexcept{
       bool  ret{ true };

       for(auto ch{str.begin()}; ch != str.end() ; ++ch){
          ret = play(*ch);
          if(!ret) break;
       }

       return ret;
    }

    bool Dtmf::play(const char* const ch, size_t len) const noexcept{
       bool  ret  { true };

       for(size_t idx{0}; idx<len; ++idx){
          ret = play(*(ch+idx));
          if(!ret) break;
       }

       return ret;
    }

    bool Dtmf::play(const std::vector<char>& vec, size_t len)  const noexcept{
       bool  ret  { true };

       for(size_t idx{0}; idx<len; ++idx){
          ret = play(vec.at(idx));
          if(!ret) break;
       }

       return ret;
    }

    bool Dtmf::play(const std::vector<char>& vec)  const noexcept{
       bool  ret  { true };

       for(auto el { vec.cbegin() }; el != vec.cend(); ++el){
          ret = play(*el);
          if(!ret) break;
       }

       return ret;
    }

    const string& Dtmf::getErrMsg(void) const noexcept{
          return errMsg;
    }

    bool Dtmf::play(char ch) const noexcept{
       bool  ret  { true };
       switch(ch){
              case '0':   case '1':   case '2':   case '3':
              case '4':   case '5':   case '6':   case '7':
              case '8':   case '9':   case '*':   case '#':
              case 'A':   case 'B':   case 'C':   case 'D':
                  aplay.play(get<DATA>(dtmfMap.at(ch)), errMsg);
              break;
              case 'a':   case 'b':   case 'c':   case 'd':
                  aplay.play(get<DATA>(dtmfMap.at(toupper(ch))), errMsg);
              break;
              case ' ':
                  aplay.play(white, errMsg);
              break;
          default:
                  ret  =  false;
       }

       if(whiteSound)
             aplay.play(white, errMsg);

       return ret;
    }

} // End Namespace
