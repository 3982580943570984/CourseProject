#include "ApplicationTemplate.hpp"

#include <string>

#include "imgui.h"

class Application : public ApplicationTemplate {
        public:
                Application () = default;
                ~Application () = default;

                virtual void StartUp ();
                virtual void Update ();

                void centeredText (const std::string& text);
};
