//
// Stronger 64-bit hash function helper, as described here: http://www.javamex.com/tutorials/collections/strong_hash_code_implementation.shtml
// @author raver119@gmail.com
//

#ifndef LIBND4J_HELPER_HASH_H
#define LIBND4J_HELPER_HASH_H

#include <string>
#include <pointercast.h>
#include <mutex>

namespace nd4j {
    namespace ops {
        class HashHelper {
        private:
            static HashHelper* _INSTANCE;

            Nd4jIndex _byteTable[256];
            const Nd4jIndex HSTART = 0xBB40E64DA205B064L;
            const Nd4jIndex HMULT = 7664345821815920749L;

            bool _isInit = false;
            std::mutex _locker;


        public:

            static HashHelper* getInstance() {
                if (_INSTANCE == 0)
                    _INSTANCE = new HashHelper();

                return _INSTANCE;
            }

            Nd4jIndex getLongHash(std::string& str) {
                _locker.lock();
                if (!_isInit) {
                    nd4j_verbose("Building HashUtil table\n","");

                    Nd4jIndex h = 0x544B2FBACAAF1684L;
                    for (int i = 0; i < 256; i++) {
                        for (int j = 0; j < 31; j++) {
                            h = (((unsigned long long) h) >> 7) ^ h;
                            h = (h << 11) ^ h;
                            h = (((unsigned long long) h) >> 10) ^ h;
                        }
                        _byteTable[i] = h;
                    }


                    _isInit = true;
                }

                _locker.unlock();

                Nd4jIndex h = HSTART;
                Nd4jIndex hmult = HMULT;
                Nd4jIndex len = str.size();
                for (int i = 0; i < len; i++) {
                    char ch = str.at(i);
                    auto uch = (unsigned char) ch;
                    h = (h * hmult) ^ _byteTable[ch & 0xff];
                    h = (h * hmult) ^ _byteTable[(uch >> 8) & 0xff];
                }

                return h;
            }
        };
    }
}

nd4j::ops::HashHelper* nd4j::ops::HashHelper::_INSTANCE = 0;

#endif //LIBND4J_HELPER_HASH_H
