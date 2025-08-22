
#include <cstdlib>
#include <ctime>

#include "uuid.hpp"



void AM::UUID::generate() {
    std::srand(std::time({}));

    
    for(size_t i = 0; i < AM::UUID_LENGTH; i++) {
        m_uuid[i] = (std::rand() % 255);
    }

}
        

uint8_t AM::UUID::operator[](size_t index) {
    return (index < AM::UUID_LENGTH) ? m_uuid[index] : 0;
}

bool AM::UUID::equals(UUID& rh) {
    for(size_t i = 0; i < AM::UUID_LENGTH; i++) {
        
        if(m_uuid[i] != rh[i]) {
            return false;
        }
        
    }
    return true;
}



