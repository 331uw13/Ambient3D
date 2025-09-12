
#include <iostream>
#include "item_manager.hpp"

void AM::ItemManager::update() {

    for(auto it = m_item_map.begin(); it != m_item_map.end(); ++it) {
    
    }

}

void AM::ItemManager::add_itembase(const ItemBase& itembase) {
    const auto item_search = m_item_map.find(itembase.uuid);
    if(item_search == m_item_map.end()) {
        AM::Item item = (AM::Item)itembase;
        item.inactive_time = 0;

        std::string model_path = m_item_list[item.entry_name]["model_path"]
            .template get<std::string>();

        //item.renderable.load(, { m_item_shader });
    
        if(item.renderable.is_loaded()) {
            printf("[ITEM_MANAGER]: Loaded item (%s)\n", item.entry_name);
            m_item_map.insert(std::make_pair(item.uuid, item));
        }
    }
    else {
       item_search->second.pos_x = itembase.pos_x; 
       item_search->second.pos_y = itembase.pos_y; 
       item_search->second.pos_z = itembase.pos_z; 
    }
}

void AM::ItemManager::free_everything() {
}

