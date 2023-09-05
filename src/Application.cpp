#include "Application.h"
#include "Window.h"
#include <AK/OwnPtr.h>
#include <LibWebView/CookieJar.h>
#include <glib/gi18n.h>

struct _LadybirdApplication {
    AdwApplication parent_instance;

    OwnPtr<WebView::CookieJar> cookie_jar;
    OwnPtr<WebView::CookieJar> incognito_cookie_jar;
    GFile* webdriver_content_ipc_path;
};

G_BEGIN_DECLS

G_DEFINE_FINAL_TYPE(LadybirdApplication, ladybird_application, ADW_TYPE_APPLICATION)

static void open_new_window(GSimpleAction* action, [[maybe_unused]] GVariant* state, void* user_data)
{
    LadybirdApplication* app = LADYBIRD_APPLICATION(user_data);
    char const* action_name = g_action_get_name(G_ACTION(action));
    bool incognito = !strcmp(action_name, "new-incognito-window");
    LadybirdWindow* window = ladybird_window_new(app, true, incognito);
    gtk_window_present(GTK_WINDOW(window));
}

static void show_shortcuts([[maybe_unused]] GSimpleAction* action, [[maybe_unused]] GVariant* state, void* user_data)
{
    GtkApplication* app = GTK_APPLICATION(user_data);
    GtkBuilder* builder = gtk_builder_new_from_resource("/org/serenityos/Ladybird-gtk4/shortcuts-dialog.ui");
    GtkWindow* dialog = GTK_WINDOW(gtk_builder_get_object(builder, "shortcuts_dialog"));
    gtk_window_set_transient_for(dialog, gtk_application_get_active_window(app));
    gtk_window_present(GTK_WINDOW(dialog));
    g_object_unref(builder);
}

static void show_about([[maybe_unused]] GSimpleAction* action, [[maybe_unused]] GVariant* state, void* user_data)
{
    GtkApplication* app = GTK_APPLICATION(user_data);

    char const* developers[] = {
        /* Translators: Сергей Бугаев in Cyrillic */
        _("Sergey Bugaev"),
        nullptr
    };

    char const* artists[] = {
        _("Sam Atkins"),
        nullptr
    };

    AdwAboutWindow* about_window = ADW_ABOUT_WINDOW(g_object_new(ADW_TYPE_ABOUT_WINDOW,
        "application-name", "Ladybird",
        "version", "WIP",
        "application-icon", "org.serenityos.Ladybird-gtk4",
        "developer-name", _("SerenityOS developers"),
        "website", "https://ladybird.dev",
        "issue-url", "https://github.com/SerenityOS/serenity/issues",
        "copyright", _("© 2023 SerenityOS developers"),
        "license-type", GTK_LICENSE_BSD,
        "developers", developers,
        "artists", artists,
        // Translators: credit yourself here :^)
        "translator-credits", _("translator_credits"),
        "comments", _("Ladybird is a browser based on LibWeb web engine and LibJS JavaScript engine,"
                      " developed by a large team of contributors as a part of the SerenityOS project."),
        nullptr));

    adw_about_window_add_link(about_window, _("SerenityOS website"), "https://serenityos.org");

    GtkWindow* active_window = gtk_application_get_active_window(app);
    if (active_window)
        gtk_window_set_transient_for(GTK_WINDOW(about_window), active_window);
    gtk_window_present(GTK_WINDOW(about_window));
}

static void do_quit([[maybe_unused]] GSimpleAction* action, [[maybe_unused]] GVariant* state, void* user_data)
{
    GApplication* app = G_APPLICATION(user_data);
    g_application_quit(app);
}

static GActionEntry const app_entries[] = {
    { "new-window", open_new_window, nullptr, nullptr, nullptr, { 0 } },
    { "new-incognito-window", open_new_window, nullptr, nullptr, nullptr, { 0 } },
    { "shortcuts", show_shortcuts, nullptr, nullptr, nullptr, { 0 } },
    { "about", show_about, nullptr, nullptr, nullptr, { 0 } },
    { "quit", do_quit, nullptr, nullptr, nullptr, { 0 } }
};

static char const* const new_window_accels[] = { "<Primary>n", nullptr };
static char const* const new_incognito_window_accels[] = { "<Primary><shift>n", nullptr };
static char const* const shortcuts_accels[] = { "<Primary>question", nullptr };
static char const* const quit_accels[] = { "<Primary>q", nullptr };

static void ladybird_application_activate(GApplication* app)
{
    // Chain up (for no good reason...)
    G_APPLICATION_CLASS(ladybird_application_parent_class)->activate(app);

    LadybirdWindow* window = ladybird_window_new(LADYBIRD_APPLICATION(app), true, false);
    gtk_window_present(GTK_WINDOW(window));
}

static void ladybird_application_open(GApplication* app, GFile** files, int num_files, char const* hint)
{
    // Chain up (for no good reason...)
    G_APPLICATION_CLASS(ladybird_application_parent_class)->open(app, files, num_files, hint);

    // Look for a window to add the tabs to.
    LadybirdWindow* window = LADYBIRD_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    if (!window)
        window = ladybird_window_new(LADYBIRD_APPLICATION(app), false, false);

    for (int i = 0; i < num_files; i++)
        ladybird_window_open_file(window, files[i]);

    gtk_window_present(GTK_WINDOW(window));
}

static gint ladybird_application_handle_local_options(GApplication* app, GVariantDict* options)
{
    LadybirdApplication* self = LADYBIRD_APPLICATION(app);

    char const* webdriver_content_ipc_path = nullptr;
    if (g_variant_dict_lookup(options, "webdriver-content-path", "ay", &webdriver_content_ipc_path)) {
        g_assert(self->webdriver_content_ipc_path == nullptr);
        self->webdriver_content_ipc_path = g_file_new_for_commandline_arg(webdriver_content_ipc_path);
    }

    return G_APPLICATION_CLASS(ladybird_application_parent_class)->handle_local_options(app, options);
}

WebView::CookieJar* ladybird_application_get_cookie_jar(LadybirdApplication* self)
{
    g_return_val_if_fail(LADYBIRD_IS_APPLICATION(self), nullptr);

    if (!self->cookie_jar) {
        // TODO: This should attempt creating the jar from a database.
        auto jar = WebView::CookieJar::create();
        self->cookie_jar = make<WebView::CookieJar>(move(jar));
    }

    return self->cookie_jar;
}

WebView::CookieJar* ladybird_application_get_incognito_cookie_jar(LadybirdApplication* self)
{
    g_return_val_if_fail(LADYBIRD_IS_APPLICATION(self), nullptr);

    if (!self->incognito_cookie_jar) {
        auto jar = WebView::CookieJar::create();
        self->incognito_cookie_jar = make<WebView::CookieJar>(move(jar));
    }

    return self->incognito_cookie_jar;
}

GFile* ladybird_application_get_webdriver_content_ipc_path(LadybirdApplication* self)
{
    g_return_val_if_fail(LADYBIRD_IS_APPLICATION(self), nullptr);

    return self->webdriver_content_ipc_path;
}

static void ladybird_application_dispose(GObject* object)
{
    LadybirdApplication* self = LADYBIRD_APPLICATION(object);

    self->cookie_jar.clear();
    self->incognito_cookie_jar.clear();
    g_clear_object(&self->webdriver_content_ipc_path);

    G_OBJECT_CLASS(ladybird_application_parent_class)->dispose(object);
}

static void ladybird_application_init([[maybe_unused]] LadybirdApplication* self)
{
    GApplication* g_app = G_APPLICATION(self);
    GtkApplication* gtk_app = GTK_APPLICATION(self);

    new (&self->cookie_jar) OwnPtr<WebView::CookieJar>;
    new (&self->incognito_cookie_jar) OwnPtr<WebView::CookieJar>;

    g_application_add_main_option(g_app, "webdriver-content-path", 0,
        G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME,
        _("Path to WebDriver IPC for WebContent"), "path");

    g_action_map_add_action_entries(G_ACTION_MAP(self), app_entries, G_N_ELEMENTS(app_entries), self);
    gtk_application_set_accels_for_action(gtk_app, "app.new-window", new_window_accels);
    gtk_application_set_accels_for_action(gtk_app, "app.new-incognito-window", new_incognito_window_accels);
    gtk_application_set_accels_for_action(gtk_app, "app.shortcuts", shortcuts_accels);
    gtk_application_set_accels_for_action(gtk_app, "app.quit", quit_accels);
}

static void ladybird_application_class_init(LadybirdApplicationClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    GApplicationClass* g_application_class = G_APPLICATION_CLASS(klass);

    object_class->dispose = ladybird_application_dispose;

    g_application_class->activate = ladybird_application_activate;
    g_application_class->open = ladybird_application_open;
    g_application_class->handle_local_options = ladybird_application_handle_local_options;
}

LadybirdApplication* ladybird_application_new(void)
{
    return LADYBIRD_APPLICATION(g_object_new(LADYBIRD_TYPE_APPLICATION,
        "application-id", "org.serenityos.Ladybird-gtk4",
        "flags", G_APPLICATION_HANDLES_OPEN,
        nullptr));
}

G_END_DECLS
