#pragma once

#include <mp-units/systems/si.h>

#include <tfc/ec/devices/schneider/atv320/enums.hpp>
#include <tfc/ec/devices/util.hpp>
#include <tfc/stx/concepts.hpp>

namespace tfc::ec::devices::schneider::atv320 {
using tfc::ec::util::setting;

using analog_input_3_range =
    setting<ecx::index_t{ 0x200E, 0x55 }, "AI3L", "Analog input 3 range", aiol_e, aiol_e::positive_only>;

using configuration_of_AI1 =
    setting<ecx::index_t{ 0x2010, 0x02 }, "AI1", "Configuration of Analog input 1", aiot_e, aiot_e::current>;
using configuration_of_AI3 =
    setting<ecx::index_t{ 0x200E, 0x05 }, "AI3", "Configuration of Analog input 3", aiot_e, aiot_e::current>;
using configuration_reference_frequency_1_FR1 = setting<ecx::index_t{ 0x2036, 0xE },
                                                        "FR1",
                                                        "Configuration reference frequency 1",
                                                        psa_e,
                                                        psa_e::reference_frequency_via_com_module>;

using assignment_AQ1 = setting<ecx::index_t{ 0x2014, 0x16 }, "AO1", "AQ1 assignment", psa_e, psa_e::not_configured>;

using assignment_R1 = setting<ecx::index_t{ 0x2014, 0x02 }, "AO1", "AQ1 assignment", psl_e, psl_e::not_assigned>;

// ATV320 specific data type and multiplier for unit
using decifrequency = mp_units::quantity<mp_units::si::deci<mp_units::si::hertz>, uint16_t>;
using decifrequency_signed = mp_units::quantity<mp_units::si::deci<mp_units::si::hertz>, int16_t>;
using decawatt = mp_units::quantity<mp_units::si::deca<mp_units::si::watt>, uint16_t>;
using atv_deciampere_rep = mp_units::quantity<mp_units::si::deci<mp_units::si::ampere>, uint16_t>;

using namespace mp_units::si::unit_symbols;

// Units 0.01 KW / 10W
using nominal_motor_power_NPR = setting<ecx::index_t{ 0x2042, 0x0E }, "NPR", "Nominal motor power", decawatt, 15 * hW>;
using nominal_motor_voltage_UNS = setting<ecx::index_t{ 0x2042, 0x02 },
                                          "UNS",
                                          "Nominal motor voltage",
                                          mp_units::quantity<mp_units::si::volt, uint16_t>,
                                          400 * V>;

using nominal_motor_frequency_FRS =
    setting<ecx::index_t{ 0x2042, 0x03 }, "FRS", "Nominal motor frequency", decifrequency, 500 * dHz>;

using fast_stop_ramp_divider_DCF =
    setting<ecx::index_t{ 0x2052, 0x1F },
            "DCF",
            "Fast stop ramp divider. Controls the quick stop time documented as a divider from 1-10. 0 seems to be instant.",
            uint16_t,
            4>;

using nominal_motor_current_NCR =
    setting<ecx::index_t{ 0x2042, 0x04 }, "NCR", "Nominal motor current", atv_deciampere_rep, 20 * dA>;

using nominal_motor_speed_NSP = setting<ecx::index_t{ 0x2042, 0x05 }, "NSP", "Nominal motor speed", uint16_t, 1500>;

// Units 0.01
using motor_1_cos_phi_COS = setting<ecx::index_t{ 0x2042, 0x07 }, "COS", "Motor 1 cosinus phi", uint16_t, 80>;

using motor_thermal_current_ITH =
    setting<ecx::index_t{ 0x2042, 0x17 }, "ITH", "motor thermal current", atv_deciampere_rep, 20 * dA>;

using current_limitation_CLI =
    setting<ecx::index_t{ 0x203E, 0x2 }, "CLI", "Current limitation", atv_deciampere_rep, 20 * dA>;

//  Units 0.1 Hz, Range 10Hz - 500Hz

using max_frequency_TFR = setting<ecx::index_t{ 0x2001, 0x04 }, "TFR", "Max frequency", decifrequency, 800 * dHz>;
using high_speed_HSP = setting<ecx::index_t{ 0x2001, 0x05 }, "HSP", "High speed", decifrequency, 800 * dHz>;
using low_speed_LSP = setting<ecx::index_t{ 0x2001, 0x06 }, "LSP", "Low speed", decifrequency, 200 * dHz>;

using deciseconds = std::chrono::duration<uint16_t, std::deci>;
// 100 = 10 seconds
// 10 = 1 second
using acceleration_ramp_time_ACC = setting<ecx::index_t{ 0x203c, 0x02 }, "ACC", "Acceleration ramp time", deciseconds, 1>;

// 100 = 10 seconds
// 10 = 1 second
using deceleration_ramp_time_DEC = setting<ecx::index_t{ 0x203c, 0x03 }, "DEC", "Deceleration time ramp", deciseconds, 1>;

// Lenze 120Hz specific drive parameters
using async_motor_leakage_inductance_LFA =
    setting<ecx::index_t{ 0x2042, 0x3F },
            "LFA",
            "Async motor leakage inductance. Typical for 120Hz Lenze = 61mH",
            mp_units::quantity<mp_units::si::milli<mp_units::si::henry>, std::uint16_t>,
            0 * mp_units::si::henry>;
using async_motor_stator_resistance_RSA = setting<ecx::index_t{ 0x2042, 0x2B },
                                                  "RSA",
                                                  "Async motor stator resistance. Typical for 120Hz Lenze = 1100mOhm",
                                                  mp_units::quantity<mp_units::si::milli<mp_units::si::ohm>, std::uint16_t>,
                                                  0 * mp_units::si::ohm>;
using rotor_time_constant_TRA = setting<ecx::index_t{ 0x2042, 0x44 },
                                        "TRA",
                                        "Rotor time constant. Typical for 120Hz Lenze = 90ms",
                                        std::chrono::duration<std::uint16_t, std::milli>,
                                        0>;

using torque_or_current_limitation_stop_SSB =
    setting<ecx::index_t{ 0x203E, 0x29 }, "SSB", "Torque or current limit stop mode", ecfg_e, ecfg_e::no>;

// Enables us to reset more drive faults including OCF, SCF1, SCF3.
using extended_fault_reset_activation_HRFC =
    setting<ecx::index_t{ 0x2029, 0x33 }, "HRFC", "Torque or current limit stop mode", n_y_e, n_y_e::yes>;

}  // namespace tfc::ec::devices::schneider::atv320
