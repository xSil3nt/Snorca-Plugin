#ifndef slic3r_WipeTowerShapeRegistry_hpp_
#define slic3r_WipeTowerShape_hpp_

#include "IWipeTowerShape.hpp"

namespace Slic3r {

WipeTowerShapeRegistry &wipe_tower_shape_registry();
void register_builtin_wipe_tower_shapes();

std::string wipe_tower_wall_type_key(int wall_type_int);
int         wipe_tower_wall_type_from_key(const std::string &key);

} // namespace Slic3r

#endif // slic3r_WipeTowerShapeRegistry_hpp_
