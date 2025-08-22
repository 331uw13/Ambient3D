#ifndef AMBIENT3D_UUID_HPP
#define AMBIENT3D_UUID_HPP

#include <cstdint>
#include <array>

namespace AM {

    static constexpr size_t UUID_LENGTH = 32;
    static constexpr bool UUID_GEN_ON_CONSTRUCT = true;


    class UUID {
        public:
            uint8_t operator[](size_t index);

            void generate();
            bool equals(UUID& rh); 

        private:
            std::array<uint8_t, UUID_LENGTH> m_uuid;
    };

};





#endif
