
#include <iostream>
#include "item_manager.hpp"

            
AM::ItemManager::ItemManager() {
    for(size_t i = 0; i < m_item_renderables.size(); i++) {
        m_item_renderables[i] = std::make_shared<AM::Renderable>(AM::Renderable{});
    }
}
            
AM::ItemManager::~ItemManager() {
    for(size_t i = 0; i < m_item_renderables.size(); i++) {
        AM::Renderable* renderable = m_item_renderables[i].get();
        if(renderable->is_loaded()) {
            renderable->unload();
        }
    }
}


void AM::ItemManager::update_lifetimes() {
    // TODO: iterate over items and check their use_count()
    // if its 1 and some time has passed it can be unloaded.
}
 
void AM::ItemManager::m_load_item_data(AM::ItemBase* itembase) {
    itembase->load_info(m_item_list_json, itembase->id, itembase->entry_name);
    AM::Item item = (AM::Item)*itembase;
    

    AM::Renderable* renderable = m_item_renderables[item.id].get();
    if(!renderable->is_loaded()) {
        if(!renderable->load(item.model_path, { m_item_default_shader })) {
            fprintf(stderr, "[ITEM_MANAGER]: Failed to load item model \"%s\" for \"%s\"\n",
                    item.model_path, item.entry_name);
            return;
        }

        json model_settings = m_item_list_json[item.entry_name]["modelmesh_settings"];
        
        size_t mesh_index = 0;
        for(const json& mesh_settings : model_settings) {
            std::cout << mesh_settings << std::endl;
            
            const int hex_color = strtol(mesh_settings["color"].template get<std::string>().c_str(), NULL, 16);
            
            renderable->mesh_attribute(mesh_index, 
                    AM::MeshAttrib {
                        .tint = Color((hex_color >> 16) & 0xFF,
                                      (hex_color >> 8)  & 0xFF,
                                      (hex_color)       & 0xFF,
                                      255),
                        .shine = mesh_settings["shine"].template get<float>()
                    });

            mesh_index++;
            if(mesh_index > renderable->num_meshes()) {
                fprintf(stderr, "ERROR! %s: Renderable mesh count is smaller than"
                        " number of elements in \"modelmesh_settings\" in item_list.json (%s)\n",
                        __func__, item.entry_name);
                break;
            }
        }
    }

    //printf("USE_COUNT BEFORE MOVE = %li\n", m_item_renderables[item.id].use_count());
    item.renderable = m_item_renderables[item.id];
    //printf("USE_COUNT AFTER MOVE = %li\n", m_item_renderables[item.id].use_count());

    m_dropped_items.insert(std::make_pair(item.uuid, item));
    printf("[ITEM_MANAGER]: Loaded item (%s) %i\n", itembase->entry_name, itembase->uuid);
                    
}

void AM::ItemManager::m_update_item_data(AM::ItemBase* itembase) {

}


void AM::ItemManager::update_queue() {
    m_itembase_queue_mutex.lock();
    for(size_t i = 0; i < m_itembase_queue.size(); i++) {
        AM::ItemBase* itembase = &m_itembase_queue[i];
        if(itembase->id >= AM::NUM_ITEMS) {
            continue;
        }

        auto item_search = m_dropped_items.find(itembase->uuid);
        if(item_search != m_dropped_items.end()) {
            m_update_item_data(itembase);
        }
        else {
            m_load_item_data(itembase);
        }
    }

    m_itembase_queue.clear();
    m_itembase_queue_mutex.unlock();
}

void AM::ItemManager::add_itembase_to_queue(const AM::ItemBase& itembase) {
    m_itembase_queue_mutex.lock();
    m_itembase_queue.push_back(itembase);
    m_itembase_queue_mutex.unlock();
}


