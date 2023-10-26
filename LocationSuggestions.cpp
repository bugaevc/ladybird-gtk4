#include "LocationSuggestions.h"
#include "HistoryEntry.h"
#include <AK/String.h>
#include <LibWebView/SearchEngine.h>
#include <glib/gi18n.h>

struct _LadybirdLocationSuggestions {
    GObject parent_instance;

    char* text;
    AK::ReadonlySpan<WebView::SearchEngine> search_engines;
};

enum {
    PROP_0,
    PROP_ITEM_TYPE,
    PROP_N_ITEMS,
    PROP_TEXT,
    NUM_PROPS,
};

static GParamSpec* props[NUM_PROPS];

G_BEGIN_DECLS

static void ladybird_location_suggestions_init_list_model(GListModelInterface* iface);

G_DEFINE_FINAL_TYPE_WITH_CODE(LadybirdLocationSuggestions, ladybird_location_suggestions, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(G_TYPE_LIST_MODEL, ladybird_location_suggestions_init_list_model))

static void ladybird_location_suggestions_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    LadybirdLocationSuggestions* self = LADYBIRD_LOCATION_SUGGESTIONS(object);

    switch (prop_id) {
    case PROP_ITEM_TYPE:
        g_value_set_gtype(value, LADYBIRD_TYPE_HISTORY_ENTRY);
        break;

    case PROP_N_ITEMS:
        g_value_set_uint(value, self->search_engines.size());
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void ladybird_location_suggestions_set_property(GObject* object, guint prop_id, GValue const* value, GParamSpec* pspec)
{
    LadybirdLocationSuggestions* self = LADYBIRD_LOCATION_SUGGESTIONS(object);

    switch (prop_id) {
    case PROP_TEXT:
        // ladybird_location_suggestions_set_text(self, g_value_get_string(value));
        (void)value;
        (void)self;
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void ladybird_location_suggestions_finalize(GObject* object)
{
    LadybirdLocationSuggestions* self = LADYBIRD_LOCATION_SUGGESTIONS(object);

    g_clear_pointer(&self->text, g_free);
    self->search_engines.~Span();

    G_OBJECT_CLASS(ladybird_location_suggestions_parent_class)->finalize(object);
}

static void ladybird_location_suggestions_init(LadybirdLocationSuggestions* self)
{
    self->search_engines = WebView::search_engines();
}

static GType ladybird_location_suggestions_get_item_type(GListModel* model)
{
    g_return_val_if_fail(LADYBIRD_IS_LOCATION_SUGGESTIONS(model), G_TYPE_NONE);

    return LADYBIRD_TYPE_HISTORY_ENTRY;
}

static guint ladybird_location_suggestions_get_n_items(GListModel* model)
{
    LadybirdLocationSuggestions* self = LADYBIRD_LOCATION_SUGGESTIONS(model);

    return self->search_engines.size();
}

static void* ladybird_location_suggestions_get_item(GListModel* model, guint position)
{
    LadybirdLocationSuggestions* self = LADYBIRD_LOCATION_SUGGESTIONS(model);
    g_return_val_if_fail(position < self->search_engines.size(), nullptr);

    WebView::SearchEngine const& search_engine = self->search_engines[position];
    String url = MUST(String::formatted(search_engine.query_url, self->text));

    LadybirdHistoryEntry* history_entry = ladybird_history_entry_new(url.to_deprecated_string().characters());
    // FIXME: These can technically be without null termination.
    char* title = g_strdup_printf(_("Search %s for “%s”"), search_engine.name.characters_without_null_termination(), self->text);
    ladybird_history_entry_set_title(history_entry, title);
    g_clear_pointer(&title, g_free);

    return history_entry;
}

static void ladybird_location_suggestions_init_list_model(GListModelInterface* iface)
{
    iface->get_item_type = ladybird_location_suggestions_get_item_type;
    iface->get_n_items = ladybird_location_suggestions_get_n_items;
    iface->get_item = ladybird_location_suggestions_get_item;
}

static void ladybird_location_suggestions_class_init(LadybirdLocationSuggestionsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->get_property = ladybird_location_suggestions_get_property;
    object_class->set_property = ladybird_location_suggestions_set_property;
    object_class->finalize = ladybird_location_suggestions_finalize;

    constexpr GParamFlags param_flags = GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);
    constexpr GParamFlags ro_param_flags = GParamFlags(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    props[PROP_ITEM_TYPE] = g_param_spec_gtype("item-type", nullptr, nullptr, G_TYPE_OBJECT, ro_param_flags);
    props[PROP_N_ITEMS] = g_param_spec_uint("n-items", nullptr, nullptr, 0, G_MAXUINT, 0, ro_param_flags);
    props[PROP_TEXT] = g_param_spec_string("text", nullptr, nullptr, nullptr, param_flags);

    g_object_class_install_properties(object_class, NUM_PROPS, props);
}

G_END_DECLS
