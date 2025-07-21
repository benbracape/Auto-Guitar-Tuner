#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128);

#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

class OledUI {
    public:
        enum State {
            BOOT,
            MENU,
            TUNING,
            HOME
        };

        void show_tuning(String note, float cents) {
            if (cur_note != note || cur_cents != cents){
                dirty = true;
                cur_cents = cents; cur_note = note;
            }
        }

        void tick(State new_state) {
            if (state != new_state) {
                dirty = true; 
                state = new_state;
            }
            switch (state) {
                case BOOT: 
                    if (dirty) {
                        refresh_boot_display();
                        dirty = false;
                        break;
                    }
                case MENU:
                    if (dirty) {
                        refresh_menu_display();
                        dirty = false;
                        break;
                    }
                case TUNING:
                    if (dirty) {
                        refresh_tuning_display(cur_note, cur_cents);
                        dirty = false;
                        break;
                    }
                case HOME:
                    if (dirty) {
                        refresh_home_display();
                        dirty = false;
                        break;
                    }
            }
        }

    private:
        State state; 
        String cur_note; 
        float cur_cents; 
        bool dirty = false;
    
        void refresh_tuning_display(String note, float cents) {
            //display logic
        }

        void refresh_boot_display() {
            //boot display
        }

        void refresh_menu_display() {
            //menu display logic
        }

        void refresh_home_display() {
            //home display
        }
};