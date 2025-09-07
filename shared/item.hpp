#ifndef AMBIENT3D_ITEM_HPP
#define AMBIENT3D_ITEM_HPP

#include <string>
#include <cstdint>


namespace AM {

    enum ItemRarity : int {
        COMMON = 0,
        RARE,
        EPIC,
        LEGENDARY,
        MYTHICAL
    };

    struct ItemInfo {
        ItemRarity   rarity;
        std::string  display_name;
    };

    enum ItemLoadSide {
        SERVER,
        CLIENT
    };

    class ItemBase {
        public:

            // 'list_entry_name' should correspond 
            // to entry in './items/item_list.json' file
            void load_base(const char* list_entry_name);


            const ItemInfo& get_info() { return m_item_info; }

        private:

            std::string   m_model_path;
            ItemInfo      m_item_info;

    };

};


#endif
