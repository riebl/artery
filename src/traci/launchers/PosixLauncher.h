#pragma once

#include <regex>
#include <mutex>
#include <string>

#include <traci/launchers/Launcher.h>

#include <omnetpp/cobject.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cconfiguration.h>

namespace artery {

    namespace traci {

        /**
         * @brief Formatter for SUMO command line values.
         * 
         * Note, all children should be accessble for Omnet++ and therefore
         * be modules (this class itself is module interface).
         */
        class IArgumentFormatter {
        public:
            static constexpr int ERROR = -1;
            static constexpr int OK = 0;

            /**
             * @brief Format a single command line argument.
             * 
             * This method should throw if it encounters internal error and return ERROR value
             * if user provided bad input. Return OK if formatting was performed. 
             */
            virtual int format(std::string& argument) = 0;
        };

        class EnvirArgumentFormatter
            : public IArgumentFormatter
            , public omnetpp::cSimpleModule {
        public:
            EnvirArgumentFormatter();

            /* IArgumentFormatter implementation */
            virtual int format(std::string& argument) override;

        protected:
            /* omnetpp::cSimpleModule implementation */
            void initialize() override;

        private:
            std::regex pattern_;
            omnetpp::cConfigurationEx* cfg_;
        };

        class PosixLauncher 
            : public ILauncher
            , public omnetpp::cSimpleModule {
        public:
            PosixLauncher();
            ~PosixLauncher();

            /* ILauncher implementation */
            ServerEndpoint launch() override;

        protected:
            /* omnetpp::cSimpleModule implementation */
            void initialize() override;
            void finish() override;

            void cleanup(const std::string& cleanupReason);
            void appendExtraOptions(std::vector<std::string>& arguments, const std::string& extraOptions);
            void formatArguments(std::vector<std::string>& arguments);
            
        private:
            std::once_flag cleanupFlag_;

            struct SumoStartConfig {
                std::vector<std::string> args;
                std::string traceFile;
                std::string label;
                bool traceGetters;
            } startConfig_;
        };

    }

}
