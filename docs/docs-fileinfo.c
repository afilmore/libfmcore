/***********************************************************************************************************************
 * Study of FileInfo in LibFM
 * 
 * 
 * 
 * 
 **********************************************************************************************************************/

To handle file system objects, a Path structure is defined, FmPath.

struct _FmPath
{
    gint n_ref;
    FmPath* parent;
    guchar flags;       /* FmPathFlags flags : 8; */
    char name[1];
};

This Path contains some flags to represent the path.

enum _FmPathFlags
{
    FM_PATH_NONE = 0,
    FM_PATH_IS_NATIVE = 1<<0,       /* This is a native path to UNIX, like /home */
    FM_PATH_IS_LOCAL = 1<<1,        /* This path refers  to a file on local filesystem */
    FM_PATH_IS_VIRTUAL = 1<<2,      /* This path is virtual and it doesn't exist on real filesystem */
    FM_PATH_IS_TRASH = 1<<3,        /* This path is under trash:/// */
    FM_PATH_IS_XDG_MENU = 1<<4,     /* This path is under menu:/// */

    /* reserved for future use */
    FM_PATH_IS_RESERVED1 = 1<<5,
    FM_PATH_IS_RESERVED2 = 1<<6,
    FM_PATH_IS_RESERVED3 = 1<<7,
};

GIO defines a GFILE object to handle files on the system :
http://developer.gnome.org/gio/2.28/GFile.html

For example :

GFile *file = g_file_new_for_path (g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS));

/**
 * From Glib Reference Manual :
 * 
 * These are logical ids for special directories which are defined depending on the platform used.
 * You should use g_get_user_special_dir() to retrieve the full path associated to the logical id.
 * 
 * The GUserDirectory enumeration can be extended at later date. Not every platform has a directory for
 * every logical id in this enumeration.
 *  
 *  Current User's Directories (GLib 2.32)
 * 
 *  The user's Desktop directory:       G_USER_DIRECTORY_DESKTOP
 *  The user's Documents directory:     G_USER_DIRECTORY_DOCUMENTS
 *  The user's Downloads directory:     G_USER_DIRECTORY_DOWNLOAD
 *  The user's Music directory:         G_USER_DIRECTORY_MUSIC
 *  The user's Pictures directory:      G_USER_DIRECTORY_PICTURES
 *  The user's shared directory:        G_USER_DIRECTORY_PUBLIC_SHARE
 *  The user's Templates directory:     G_USER_DIRECTORY_TEMPLATES
 *  The user's Movies directory:        G_USER_DIRECTORY_VIDEOS
 *  The number of enum values:          G_USER_N_DIRECTORIES
 */
 
g_get_user_special_dir

There's also other directories :
http://developer.gnome.org/glib/2.32/glib-Miscellaneous-Utility-Functions.html



const gchar *       g_get_user_cache_dir                (void);
const gchar *       g_get_user_data_dir                 (void);
const gchar *       g_get_user_config_dir               (void);
const gchar *       g_get_user_runtime_dir              (void);
const gchar * const * g_get_system_data_dirs            (void);
const gchar * const * g_get_system_config_dirs          (void);
void                g_reload_user_special_dirs_cache    (void);

const gchar *       g_get_home_dir                      (void);
const gchar *       g_get_tmp_dir                       (void);
gchar *             g_get_current_dir                   (void);


Create a FileInfo for My Documents :

path = new Fm.Path.for_path (Environment.get_user_special_dir (UserDirectory.DOCUMENTS));
file = File.new_for_path (Environment.get_user_special_dir (UserDirectory.DOCUMENTS));
fi = new Fm.FileInfo.from_gfileinfo (path,
                                     file.query_info ("standard::*,unix::*,time::*,access::*,id::filesystem",
                                                      FileQueryInfoFlags.NONE,
                                                      null));

// in C....
gchar *path_name = g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS);
GFile *file = g_file_new_for_path (path_name);
GFileInfo *ginfo = g_file_query_info (file,
                                      "standard::*,unix::*,time::*,access::*,id::filesystem",
                                      G_FILE_QUERY_INFO_NONE,
                                      null,
                                      null);

typedef enum {
  G_FILE_QUERY_INFO_NONE              = 0,
  G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS = (1 << 0)   /*< nick=nofollow-symlinks >*/
} GFileQueryInfoFlags;

FmPath *path = fm_path_new_for_path (path_name);
FmFileInfo* fi = fm_file_info_new_from_gfileinfo(path, ginfo);

icon_name = "folder-documents";
pixbuf = icon_theme.load_icon (icon_name,
                               (int) global_config.big_icon_size,
                               Gtk.IconLookupFlags.FORCE_SIZE);

special = new Desktop.Item (pixbuf, fi);



enum _FmFileInfoFlag
{
    FM_FILE_INFO_NONE = 0,
    FM_FILE_INFO_HOME_DIR = (1 << 0),
    FM_FILE_INFO_DESKTOP_DIR = (1 << 1),
    FM_FILE_INFO_DESKTOP_ENTRY = (1 << 2),
    FM_FILE_INFO_MOUNT_POINT = (1 << 3),
    FM_FILE_INFO_REMOTE = (1 << 4),
    FM_FILE_INFO_VIRTUAL = (1 << 5),
    FM_FILE_INFO_TRASH_CAN = (1 << 6)
};


struct _FmFileInfo
{
    FmPath* path; /* path of the file */

    mode_t mode;
    union {
        const char* fs_id;
        dev_t dev;
    };
    uid_t uid;
    gid_t gid;
    goffset size;
    time_t mtime;
    time_t atime;

    gulong blksize;
    goffset blocks;

    char* disp_name;  /* displayed name (in UTF-8) */

    /* FIXME: caching the collate key can greatly speed up sorting.
     *        However, memory usage is greatly increased!.
     *        Is there a better alternative solution?
     */
    char* collate_key; /* used to sort files by name */
    char* disp_size;  /* displayed human-readable file size */
    char* disp_mtime; /* displayed last modification time */
    FmMimeType* type;
    FmIcon* icon;

    char* target; /* target of shortcut or mountable. */

    /*<private>*/
    int n_ref;
};

