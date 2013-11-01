// Header.h

#include <wx/setup.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/utils.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/xml/xml.h>

#include <igPlugin.h>

#include <list>
#include <math.h>

#include "c3ga/c3ga.h"

class Scene;
class RayTracerPlugin;
class Quadratic;
class AmbientLight;
class PointLight;
class Sphere;
class Plane;
class Quadric;

#include "Scene.h"
#include "RayTracer.h"
#include "Quadratic.h"
#include "Lights/AmbientLight.h"
#include "Lights/PointLight.h"
#include "Objects/Sphere.h"
#include "Objects/Plane.h"
#include "Objects/Quadric.h"

// Header.h