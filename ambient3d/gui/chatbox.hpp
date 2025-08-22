#ifndef AMBIENT3D_CHATBOX_HPP
#define AMBIENT3D_CHATBOX_HPP


#include <vector>
#include <string>
#include <cstdint>
#include <raylib.h>


namespace AM {

    struct ChatMsg {
        Color        color;
        std::string  data;
    };

    // TODO: Have a "GUIModule"
    // the engine will handle the focus automatically
    // when its told how it can gain a focus: Key press Or a function call.
    // the engine then will forward all character key presses to the module.
    // and module will get a callback.
    //
    // Gui stuff like this chatbox will extend the GUIModule class.
    // would be maybe cool if the key_press_event could have override ?

    class Chatbox {
        public:
            uint32_t width  { 500 };
            uint32_t height { 200 };

            void render(Font* font, int font_size);

            void push_message(const Color& color, const std::string& msg);



        private:

            std::vector<ChatMsg> m_msg_buffer;

    };
};





#endif
