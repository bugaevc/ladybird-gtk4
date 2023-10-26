#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(LadybirdLocationSuggestions, ladybird_location_suggestions, LADYBIRD, LOCATION_SUGGESTIONS, GObject)
#define LADYBIRD_TYPE_LOCATION_SUGGESTIONS ladybird_location_suggestions_get_type()

G_END_DECLS
