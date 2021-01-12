// -----------------------------------------------------------------
// dtmf - a tool using dtmflib
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
#include <parseCmdLine.hpp>
#include <config.h>

using namespace std;
using namespace dtmfutil;
using namespace parcmdline;

void paramError(const char* progname, const char* err)  noexcept    __attribute__ ((noreturn));
void versionInfo(void)                                  noexcept    __attribute__ ((noreturn));

int main(int argc, char **argv) {

   int       ret { 0 };

   try{

       if(argc != 1){
           constexpr char    flags[]    { "c:b:t:o:s:hV" };
           ParseCmdLine      pcl(argc, argv, flags);
           if(pcl.getErrorState()){
               string exitMsg{string("Invalid  parameter or value").append(pcl.getErrorMsg())};
               paramError(argv[0], exitMsg.c_str());
           }
    
           if(pcl.isSet('h')) paramError(argv[0], "");
           if(pcl.isSet('V')) versionInfo();
    
            if( !pcl.isSet('b')  &&
                !pcl.isSet('c')  &&
                !pcl.isSet('t')  &&
                !pcl.isSet('o')  &&
                !pcl.isSet('s')  &&
                !pcl.isSet('h')  &&
                !pcl.isSet('V')) 
                     paramError(argv[0], "Invalid Parameter(s).");
    
           if( (pcl.isSet('s') && pcl.isSet('b')) ||
               (pcl.isSet('s') && pcl.isSet('c')) ||
               (pcl.isSet('s') && pcl.isSet('t')) ||
               (pcl.isSet('s') && pcl.isSet('o')) )
                     paramError(argv[0], "-s isn't compatible with these options: -b, -c, -t, -o.");

           if( (pcl.isSet('b') && !pcl.isSet('o')) ||
               (pcl.isSet('c') && !pcl.isSet('o')) ||
               (pcl.isSet('t') && !pcl.isSet('o')) ||
               (pcl.isSet('o') && !( pcl.isSet('b') ||  pcl.isSet('c') || pcl.isSet('t'))))
                     paramError(argv[0], "-b, -c or -t requires -o and vice versa.");
    
           if(pcl.isSet('s')){
               Dtmf  dtmf;
               dtmf.init();
               bool res { dtmf.play(pcl.getValue('s')) };
               if(!res) 
                  cerr << "Player error: " << dtmf.getErrMsg() << endl;
           }
    
           if(pcl.isSet('b')){
               BinFileToDtmf ftd(pcl.getValue('b'));
               ftd.saveTo(pcl.getValue('o'));
           }
    
           if(pcl.isSet('c')){
               BinFileToMT8870Dtmf etd(pcl.getValue('c'));
               etd.saveTo(pcl.getValue('o'));
           }
    
           if(pcl.isSet('t')){
               DtmfToBinFile dtf(pcl.getValue('t'));
               dtf.saveTo(pcl.getValue('o'));
           }
    
       }else{
           vector<char> buff(1024);
           fill(buff.begin(), buff.end(), 0);
    
           Dtmf  dtmf;
           dtmf.init();
    
           while(true){
                ssize_t  res   { read(STDIN_FILENO, buff.data(), buff.size()) };
                cerr  << "Read len: " << res << endl;
                if(res > 0){
                    bool bres { dtmf.play(buff.data(), res) };
                    if(!bres) 
                        cerr << "Player error: " << dtmf.getErrMsg() << endl;
                }else{
                    ret = res < 0 ? 1 : 0;
                    break;
                }
           }
       }
   }catch(string& err){
       cerr << "Error: " << err << endl;
       ret  =  1;

   }catch(...){
       cerr << "Unexpected Error.\n";
       ret  =  1;
   }

   return ret;
}

void paramError(const char* progname, const char* err) noexcept{

   if(err != nullptr) cerr << err << "\n\n";

   cerr << progname   << " - a cmd line dtmf tool.\n"
                      << " GBonacini - (C) 2019 \n"  
        << "Syntax: \n"   
        << "       "  << progname   << "\n" 
        << "       "  << "   The program will reproduce a sequence of ascii tone     \n"   
        << "       "  << "   representation (01234567890ABCD#*) received on stdin.   \n"   
        << "  or   \n"
        << "       "  << progname << " [ -b input file to convert] [-t tone file] \n" 
        << "       "              << " [-c input file to convert]\n" 
        << "       "              << " [-o output file] [-s string to play]\n"
        << "       "              << " | [-h] | [-V]  \n\n" 
        << "       "  << "-b input file to convert.                                    \n" 
        << "       "  << "   It is a regular file (binary or text) that will be        \n" 
        << "       "  << "   converted in tone coding.                                 \n" 
        << "       "  << "-c input file to convert.                                    \n" 
        << "       "  << "   It is a regular file (binary or text) that will be        \n" 
        << "       "  << "   converted in tone coding for MT8870 IOT adapter           \n" 
        << "       "  << "-t tone file.                                                \n" 
        << "       "  << "   It contains a sequence of tones that will be converted    \n" 
        << "       "  << "   back to the initial file (binary or text).                \n" 
        << "       "  << "-o output file.                                              \n" 
        << "       "  << "   This flag is necessary if -b or -t is specified.          \n" 
        << "       "  << "   This file will contain the encoded/decode data.           \n" 
        << "       "  << "-s A string with one or more ascii representation of tones   \n" 
        << "       "  << "   (01234567890ABCD#*) that will be reproduced by sound card \n"
        << "       "  << "-h print this help message. \n"                                    
        << "       "  << "-V version information.";

   exit(1);
}

void versionInfo(void) noexcept{
   cerr << PACKAGE << " version: " VERSION << endl;
   exit(1);
}