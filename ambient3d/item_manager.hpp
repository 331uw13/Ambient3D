#ifndef AMBIENT3D_ITEM_MANAGER_HPP
#define AMBIENT3D_ITEM_MANAGER_HPP


// Item manager handles loading and unloading received items.
// Their 3D models, inventory textures and item type specific information.
//
// Item may be unloaded from memory if its very far away
// or its not been seen in a while

#include <map>
#include <array>
#include <nlohmann/json.hpp>

#include "raylib.h"
#include "item.hpp"

using json = nlohmann::json;


namespace AM {

    class ItemManager {
        public:

            void update();
            void add_itembase(const ItemBase& itembase);
            void free_everything();

            void assign_item_list(const json& item_list) { m_item_list = item_list; }
            void set_item_shader(const Shader& shader) { m_item_shader = shader; }

        private:

            // TODO: 
            // store item renderables in array.
            // unload and load them when needed
            // then items should have pointers to this array.
            std::array<>

            Shader m_item_shader;
            json m_item_list;

            std::map<int/* item uuid*/, Item> m_item_map;

    };


};


#endif
