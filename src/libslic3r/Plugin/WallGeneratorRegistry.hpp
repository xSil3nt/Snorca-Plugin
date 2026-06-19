#ifndef slic3r_WallGeneratorRegistry_hpp_
#define slic3r_WallGeneratorRegistry_hpp_

#include "IWallGenerator.hpp"

namespace Slic3r {

WallGeneratorRegistry &wall_generator_registry();
void register_wall_generator_defaults();

} // namespace Slic3r

#endif // slic3r_WallGeneratorRegistry_hpp_
