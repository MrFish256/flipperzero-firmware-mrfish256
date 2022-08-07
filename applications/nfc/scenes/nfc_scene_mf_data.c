#include "../nfc_i.h"

enum {
    ReadMifareUlStateShowUID,
    ReadMifareUlStateShowData,
};

bool nfc_mf_data_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return true;
}

void nfc_scene_mf_data_on_enter(void* context) {
    Nfc* nfc = context;

    MfUltralightData* mf_ul_data = &nfc->dev->dev_data.mf_ul_data;

    // Setup TextBox view
    TextBox* text_box = nfc->text_box;
    text_box_set_font(text_box, TextBoxFontHex);
    int8_t sectorIndex = 0;

    // Clean views
    text_box_reset(nfc->text_box);
    string_reset(nfc->text_box_store);

    // Data TEXT mode
    if(nfc->text_input_mode == 0) {
	for(uint16_t i = 0; i < mf_ul_data->data_size; i += 1) {
                char c = (char)mf_ul_data->data[i];
                
                if(isprint(c) != 0) {
                        string_cat_printf(
                            nfc->text_box_store, "%c", c);
                } else {
			if(c == 10 || c == 13) {
	                        string_push_back(nfc->text_box_store, '\n');
			}
                }
        }
    // Data SECTOR TEXT mode
    } else if(nfc->text_input_mode == 1) {
	string_cat_printf(
        nfc->text_box_store, "[%02X] ", sectorIndex);

	for(uint16_t i = 0; i < mf_ul_data->data_size; i += 1) {
        	if(!(i % 8) && i) {
            		sectorIndex += 1;
           		string_push_back(nfc->text_box_store, '\n');
            		string_cat_printf(
                		nfc->text_box_store, "[%02X] ", sectorIndex);
        	}

	        char c = (char)mf_ul_data->data[i];

	        if(isprint(c) != 0) {
        	        string_cat_printf(
                	    nfc->text_box_store, "%c", c);
	        } else {
        	    string_cat_printf(
                	    nfc->text_box_store, ".");
	        }
	}
    }
    
    string_cat_printf(
 	nfc->text_box_store, "[%02X] ", sectorIndex);

    for(uint16_t i = 0; i < mf_ul_data->data_size; i += 1) {
        if(!(i % 8) && i) {
	    sectorIndex += 1;
            string_push_back(nfc->text_box_store, '\n');
    	    string_cat_printf(
 		nfc->text_box_store, "[%02X] ", sectorIndex);
        }

	char c = (char)mf_ul_data->data[i];

	if(isprint(c) != 0) {
	        string_cat_printf(
        	    nfc->text_box_store, "%c", c);
	} else {
    	    string_cat_printf(
        	    nfc->text_box_store, ".");
	}
    }
    text_box_set_text(text_box, string_get_cstr(nfc->text_box_store));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
}


bool nfc_scene_mf_data_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    UNUSED(nfc);
    if(event.type == SceneManagerEventTypeBack) {
        consumed = false;
    }
    return consumed;

}

void nfc_scene_mf_data_on_exit(void* context) {
    Nfc* nfc = context;

    // Clean views
    dialog_ex_reset(nfc->dialog_ex);
    text_box_reset(nfc->text_box);
    string_reset(nfc->text_box_store);

    popup_reset(nfc->popup);
}

