#include "lights_switch.h"

namespace esphome
{
    namespace balboa_spa
    {
        void LightsSwitch::update(SpaState *spaState)
        {
            if (!spa->has_status() || spa->control_pending(ConfirmedCommands::LIGHT)) return;
            if (this->state != spaState->light)
            {
                this->publish_state(spaState->light);
            }
        }

        void LightsSwitch::set_parent(BalboaSpa *parent)
        {
            spa = parent;
            parent->register_listener([this](SpaState *spaState)
                                      { this->update(spaState); });
        }

        void LightsSwitch::write_state(bool state)
        {
            spa->request_control(ConfirmedCommands::LIGHT, state);
            this->publish_state(state);
        }

    } // namespace balboa_spa
} // namespace esphome
