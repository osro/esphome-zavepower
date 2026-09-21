#include "highrange_switch.h"

namespace esphome
{
    namespace balboa_spa
    {
        void HighrangeSwitch::update(SpaState *spaState)
        {
            if (!spa->has_status() || spa->control_pending(ConfirmedCommands::HIGH_RANGE)) return;
            if (this->state != spaState->highrange)
            {
                this->publish_state(spaState->highrange);
            }
        }

        void HighrangeSwitch::set_parent(BalboaSpa *parent)
        {
            spa = parent;
            parent->register_listener([this](SpaState *spaState)
                                      { this->update(spaState); });
        }

        void HighrangeSwitch::write_state(bool state)
        {
            spa->set_highrange(state);
            this->publish_state(state);
        }

    } // namespace balboa_spa
} // namespace esphome
