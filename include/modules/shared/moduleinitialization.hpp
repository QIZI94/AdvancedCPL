#pragma once
#include <vector>
#include <memory>
#include "module.hpp"
namespace acpl{
namespace modules{
namespace shared{
using ModuleUniqueList = std::vector<std::unique_ptr<shared::Module>>;
ModuleUniqueList InitializeModules();

}}}