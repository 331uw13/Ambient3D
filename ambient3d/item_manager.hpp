#ifndef AMBIENT3D_ITEM_MANAGER_HPP
#define AMBIENT3D_ITEM_MANAGER_HPP


// Item manager handles loading and unloading received items.
// Their 3D models, inventory textures and item type specific information.


#include <map>
#include <array>
#include <nlohmann/json.hpp>
#include <deque>
#include <mutex>
#include <unordered_map>

#include "raylib.h"
#include "item.hpp"

using json = nlohmann::json;


namespace AM {

    class ItemManager {
        public:

            ItemManager();
            ~ItemManager();
            void free();

            void update_lifetimes();
            void update_queue();
            void add_itembase_to_queue(const AM::ItemBase& itembase);

            void set_item_default_shader(const Shader& shader) { m_item_default_shader = shader; }
            void assign_item_list(const json& item_list) { m_item_list_json = item_list; }

            const std::unordered_map<int, AM::Item>* 
                get_dropped_items() { return &m_dropped_items; }

        private:
           
            void     m_load_item_data(AM::ItemBase* itembase);
            void     m_update_item_data(AM::ItemBase* itembase);

            Shader                   m_item_default_shader;
            json                     m_item_list_json;

            std::mutex               m_itembase_queue_mutex;
            std::deque<AM::ItemBase> m_itembase_queue;
            
            // Item renderables(models) are loaded
            // if client receives items (see ITEM_UPDATE packet)
            // and is not loaded yet.
            
            std::array<std::shared_ptr<AM::Renderable>, AM::NUM_ITEMS> m_item_renderables;
            std::unordered_map<int/* item uuid */, AM::Item> m_dropped_items;
            


    };


};


#endif
