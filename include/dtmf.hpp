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

#ifndef DTMF_BG_LIB___
#define DTMF_BG_LIB___

#include <cctype>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <algorithm>

#include <unistd.h>

#include <alsa/asoundlib.h>
#include <alsa/control.h>

namespace dtmfutil {

    using  RawSoundArray  =  std::vector<unsigned char>;
    using  DtmfAttrib     =  std::tuple<int, int, RawSoundArray>;
    enum   DtmfAttribIdx  {  TONE_1, TONE_2, DATA };
    using  DtmfMap        =  std::map<char, DtmfAttrib>;

    class AlsaPlay{
        public:
            AlsaPlay(size_t sSize, std::string dev="default");
            ~AlsaPlay(void);

            bool play(const RawSoundArray& arr, 
                      std::string& errmsg)                              const   noexcept;

        private:
            snd_pcm_t*                  handle;
            mutable snd_pcm_sframes_t   frames;
            std::string                 device;
            size_t                      sampleSize;
    };

    class Dtmf{
        public:
            explicit             Dtmf(size_t sSize      =  8000,
                                      size_t whiteFract =  40,
                                      bool   whsound    =  true);
            void                 init(size_t tlen       =  250)                 noexcept;
            bool                 play(const std::string& str)           const   noexcept;
            bool                 play(char ch)                          const   noexcept;
            bool                 play(const char* const ch,
                                      size_t            len)            const   noexcept;
            bool                 play(const std::vector<char>& vec,
                                      size_t            len)            const   noexcept;
            bool                 play(const std::vector<char>& vec)     const   noexcept;

            static float         bgsine(short in)                               noexcept;
            const std::string&   getErrMsg(void)                        const   noexcept;

        private:
            size_t                sampleSize,
                                  sampleNum;
            DtmfMap               dtmfMap;
            AlsaPlay              aplay;
            RawSoundArray         white;
            bool                  whiteSound;
            mutable std::string   errMsg;

            friend std::istream& operator>> (std::istream& in, const Dtmf& dtmf);
        };

    class BinFileToDtmf{
        public:
           explicit BinFileToDtmf(const std::string& fileName);
           ~BinFileToDtmf(void);

           bool saveTo(const std::string& outFile)                 const  noexcept;

        private:
           std::ifstream                 iFile;
           mutable std::ofstream         oFile;
           const std::array<char, 0x10>  binToDtmf;

           friend std::ostream& operator<< (std::ostream& out, const BinFileToDtmf& ftd);
    };

    class DtmfToBinFile{
        public:
           explicit DtmfToBinFile(const std::string& fileName);
           ~DtmfToBinFile(void);

           bool saveTo(const std::string& outFile)             const  noexcept;

        private:
           std::ifstream                 iFile;
           mutable std::ofstream         oFile;
           const std::map<char, uint8_t> dtmfToBin;

           friend std::ostream& operator<< (std::ostream& out, const DtmfToBinFile& ftd);
    };

} // End Namespace

#endif
