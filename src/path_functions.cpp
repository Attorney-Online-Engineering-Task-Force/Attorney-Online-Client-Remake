#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"

#include <QDir>
#include <QStandardPaths>
#include <QRegExp>

#ifdef BASE_OVERRIDE
#include "base_override.h"
#endif

#ifndef ASSET_ENV_VAR
#define ASSET_ENV_VAR "AO2_ASSET_PATH"
#endif

//this is a quite broad generalization
//the most common OSes(mac and windows) are _usually_ case insensitive
//however, there do exist mac installations with case sensitive filesystems
//in that case, define CASE_SENSITIVE_FILESYSTEM and compile on a mac
#if (defined (LINUX) || defined (__linux__))
#define CASE_SENSITIVE_FILESYSTEM
#endif

QString AOApplication::get_default_path() {
  QString base_path = "";
#ifdef ANDROID
  QString sdcard_storage = getenv("SECONDARY_STORAGE");
  if (dir_exists(sdcard_storage + "/AO2/")){
    base_path = sdcard_storage + "/AO2/";
  }
  else {
    QString external_storage = getenv("EXTERNAL_STORAGE");
    base_path = external_storage + "/AO2/";
  }
#else
  base_path = applicationDirPath() + "/base/";
#endif
  return base_path;
}

QStringList AOApplication::get_path_list()
{
  QStringList paths;
  QString assets_env = qgetenv(ASSET_ENV_VAR);
  if (assets_env != NULL) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    QRegExp rx(";");
#else
    QRegExp rx(":");
#endif
    paths = assets_env.split(rx);
    for (int i = 0; i < paths.size(); i++) {
      paths[i] = QDir(paths[i]).absolutePath() + '/';
    }
  }
  // paths.push_back(get_default_path());
  return paths;
}

QString AOApplication::get_base_path(QString p_file)
{
  QString file = get_default_path() + p_file;
  for (QStringList::iterator i = asset_paths.begin(); i != asset_paths.end(); i++) {
    if (file_exists(*i + p_file)) {
      file = *i + p_file;
      break;
    }
  }
  return file;
}

QString AOApplication::get_data_path(QString p_file)
{
  return get_base_path("data/" + p_file);
}

QString AOApplication::get_default_theme_path(QString p_file)
{
  QString path = get_base_path("themes/default/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_custom_theme_path(QString p_theme, QString p_file)
{
  QString path = get_base_path("themes/" + p_theme + "/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_theme_path(QString p_file)
{
  QString path = get_base_path("themes/" + current_theme + "/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_character_path(QString p_char, QString p_file)
{
  QString path = get_base_path("characters/" + p_char + "/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_sounds_path(QString p_file)
{
  QString path = get_base_path("sounds/general/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_music_path(QString p_song)
{
  QString mp3_check = get_base_path("sounds/music/" + p_song + ".mp3");
  QString opus_check = get_base_path("sounds/music/" + p_song + ".opus");
  if (file_exists(opus_check))
  {
    #ifndef CASE_SENSITIVE_FILESYSTEM
      return get_base_path("sounds/music/" + p_song + ".opus");
    #else
      return get_case_sensitive_path(get_base_path("sounds/music/" + p_song + ".opus"));
    #endif
  }
  else if (file_exists(mp3_check))
  {
    #ifndef CASE_SENSITIVE_FILESYSTEM
      return get_base_path("sounds/music/" + p_song + ".mp3");
    #else
      return get_case_sensitive_path(get_base_path("sounds/music/" + p_song + ".mp3"));
    #endif
  }
#ifndef CASE_SENSITIVE_FILESYSTEM
  return get_base_path("sounds/music/" + p_song + ".wav");
#else
  return get_case_sensitive_path(get_base_path("sounds/music/" + p_song + ".wav"));
#endif
}

QString AOApplication::get_background_path(QString p_file)
{
  QString path = get_base_path("background/" + w_courtroom->get_current_background() + "/" + p_file);
  if (courtroom_constructed) {
#ifndef CASE_SENSITIVE_FILESYSTEM
    return path;
#else
    return get_case_sensitive_path(path);
#endif
  }
  return get_default_background_path(p_file);
}

QString AOApplication::get_default_background_path(QString p_file)
{
  QString path = get_base_path("background/default/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_evidence_path(QString p_file)
{
  QString path = get_base_path("evidence/" + p_file);
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_case_sensitive_path(QString p_file) {
  //first, check to see if it's actually there (also serves as base case for recursion)
  if (exists(p_file)) return p_file;

  QFileInfo file(p_file);

  QString file_basename = file.fileName();
  QString file_parent_dir = get_case_sensitive_path(file.absolutePath());

  //second, does it exist in the new parent dir?
  if (exists(file_parent_dir + "/" + file_basename))
    return file_parent_dir + "/" + file_basename;

  //last resort, dirlist parent dir and find case insensitive match
  QRegExp file_rx = QRegExp(file_basename, Qt::CaseInsensitive, QRegExp::FixedString);
  QStringList files = QDir(file_parent_dir).entryList();

  int result = files.indexOf(file_rx);

  if (result != -1)
    return file_parent_dir + "/" + files.at(result);

  //if nothing is found, let the caller handle the missing file
  return file_parent_dir + "/" + file_basename;
}
