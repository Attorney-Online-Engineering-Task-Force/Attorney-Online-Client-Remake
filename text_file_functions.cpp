#include "text_file_functions.h"

TextFileHandler* TextFileHandler::singleton = new TextFileHandler();

void TextFileHandler::createInstance(AOApplication *p_ao_app)
{
  singleton->ao_app = p_ao_app;

  // Create the QSettings class that points to the config.ini.
  singleton->configini = new QSettings(singleton->get_base_path() + "config.ini", QSettings::IniFormat);
}

TextFileHandler* TextFileHandler::getInstance()
{
  return singleton;
}

QString TextFileHandler::get_master_server()
{
  return configini->value("master", "").value<QString>();
}

void TextFileHandler::read_theme()
{
  QString result = configini->value("theme", "default").value<QString>();
  current_theme = result;
}

QString TextFileHandler::get_current_theme()
{
  return current_theme;
}

int TextFileHandler::read_blip_rate()
{
  int result = configini->value("blip_rate", 1).toInt();
  return result;
}

QString TextFileHandler::get_ooc_name()
{
  QString result = configini->value("ooc_name").value<QString>();
  return result;
}

int TextFileHandler::get_default_music()
{
  int result = configini->value("default_music", 50).toInt();
  return result;
}

int TextFileHandler::get_default_sfx()
{
  int result = configini->value("default_sfx", 50).toInt();
  return result;
}

int TextFileHandler::get_default_blip()
{
  int result = configini->value("default_blip", 50).toInt();
  return result;
}

int TextFileHandler::get_max_log_size()
{
  int result = configini->value("log_maximum", 200).toInt();
  return result;
}

bool TextFileHandler::get_log_goes_downwards()
{
  QString result = configini->value("log_goes_downwards", "false").value<QString>();
  return result.startsWith("true");
}

bool TextFileHandler::get_showname_enabled_by_default()
{
  QString result = configini->value("show_custom_shownames", "false").value<QString>();
  return result.startsWith("true");
}

QString TextFileHandler::get_default_username()
{
  QString result = configini->value("default_username", "").value<QString>();
  if (result.isEmpty())
      return get_ooc_name();
  else
      return result;
}

QString TextFileHandler::get_audio_output_device()
{
  QString result = configini->value("default_audio_device", "default").value<QString>();
  return result;
}

QStringList TextFileHandler::get_call_words()
{
  QStringList return_value;

  QFile callwords_ini;

  callwords_ini.setFileName(get_base_path() + "callwords.ini");

  if (!callwords_ini.open(QIODevice::ReadOnly))
    return return_value;

  QTextStream in(&callwords_ini);

  while (!in.atEnd())
  {
    QString line = in.readLine();
    return_value.append(line);
  }

  return return_value;
}

void TextFileHandler::write_to_serverlist_txt(QString p_line)
{
  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    return;
  }

  QTextStream out(&serverlist_txt);

  out << "\r\n" << p_line;

  serverlist_txt.close();
}

QVector<server_type> TextFileHandler::read_serverlist_txt()
{
  QVector<server_type> f_server_list;

  QFile serverlist_txt;
  QString serverlist_txt_path = get_base_path() + "serverlist.txt";

  serverlist_txt.setFileName(serverlist_txt_path);

  if (!serverlist_txt.open(QIODevice::ReadOnly))
  {
    return f_server_list;
  }

  QTextStream in(&serverlist_txt);

  while(!in.atEnd())
  {
    QString line = in.readLine();
    server_type f_server;
    QStringList line_contents = line.split(":");

    if (line_contents.size() < 3)
        continue;

    f_server.ip = line_contents.at(0);
    f_server.port = line_contents.at(1).toInt();
    f_server.name = line_contents.at(2);
    f_server.desc = "";

    f_server_list.append(f_server);
  }

  return f_server_list;
}

QString TextFileHandler::read_design_ini(QString p_identifier, QString p_design_path)
{
  QSettings settings(p_design_path, QSettings::IniFormat);
  QVariant value = settings.value(p_identifier);
  if (value.type() == QVariant::StringList) {
    return value.toStringList().join(",");
  } else {
    return value.toString();
  }
}

QPoint TextFileHandler::get_button_spacing(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QPoint return_value;

  return_value.setX(0);
  return_value.setY(0);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_value;
  }

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 2)
    return return_value;

  return_value.setX(sub_line_elements.at(0).toInt());
  return_value.setY(sub_line_elements.at(1).toInt());

  return return_value;
}

pos_size_type TextFileHandler::get_element_dimensions(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  pos_size_type return_value;

  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_value;
  }

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 4)
    return return_value;

  return_value.x = sub_line_elements.at(0).toInt();
  return_value.y = sub_line_elements.at(1).toInt();
  return_value.width = sub_line_elements.at(2).toInt();
  return_value.height = sub_line_elements.at(3).toInt();

  return return_value;
}

int TextFileHandler::get_font_size(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return 10;
  }

  return f_result.toInt();
}

QColor TextFileHandler::get_color(QString p_identifier, QString p_file)
{
  QString design_ini_path = get_theme_path(p_file);
  QString default_path = get_default_theme_path(p_file);
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QColor return_color(0, 0, 0);

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_color;
  }

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QColor TextFileHandler::get_chat_color(QString p_identifier, QString p_chat)
{
  p_identifier = p_identifier.prepend("c");
  QString design_ini_path = get_base_path() + "misc/" + p_chat + "/config.ini";
  QString default_path = get_base_path() + "misc/default/config.ini";
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QColor return_color(255, 255, 255);
  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_color;
  }

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
    return return_color;

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString TextFileHandler::get_sfx(QString p_identifier)
{
  QString design_ini_path = get_theme_path("courtroom_sounds.ini");
  QString default_path = get_default_theme_path("courtroom_sounds.ini");
  QString f_result = read_design_ini(p_identifier, design_ini_path);

  QString return_sfx = "";

  if (f_result == "")
  {
    f_result = read_design_ini(p_identifier, default_path);

    if (f_result == "")
      return return_sfx;
  }

  return_sfx = f_result;

  return return_sfx;
}

QString TextFileHandler::get_sfx_suffix(QString sound_to_check)
{
    QString mp3_check = get_sounds_path(sound_to_check + ".mp3");
    QString opus_check = get_sounds_path(sound_to_check + ".opus");
    if (file_exists(opus_check))
    {
        return sound_to_check + ".opus";
    }
    else if (file_exists(mp3_check))
    {
        return sound_to_check + ".mp3";
    }
    return sound_to_check + ".wav";
}

QString TextFileHandler::get_image_suffix(QString path_to_check)
{
    QString apng_check = get_sounds_path(path_to_check + ".apng");
    if (file_exists(apng_check))
    {
        return path_to_check + ".apng";
    }
    return path_to_check + ".gif";
}


//returns whatever is to the right of "search_line =" within target_tag and terminator_tag, trimmed
//returns the empty string if the search line couldnt be found
QString TextFileHandler::read_char_ini(QString p_char, QString p_search_line, QString target_tag)
{
  QSettings settings(get_character_path(p_char, "char.ini"), QSettings::IniFormat);
  settings.beginGroup(target_tag);
  QString value = settings.value(p_search_line).toString();
  settings.endGroup();
  return value;
}

QString TextFileHandler::get_char_name(QString p_char)
{
  QString f_result = read_char_ini(p_char, "name", "Options");

  if (f_result == "")
    return p_char;
  else return f_result;
}

QString TextFileHandler::get_showname(QString p_char)
{
  QString f_result = read_char_ini(p_char, "showname", "Options");
  QString f_needed = read_char_ini(p_char, "needs_showname", "Options");

  if (f_needed.startsWith("false"))
    return "";
  if (f_result == "")
    return p_char;
  else return f_result;
}

QString TextFileHandler::get_char_side(QString p_char)
{
  QString f_result = read_char_ini(p_char, "side", "Options");

  if (f_result == "")
    return "wit";
  else return f_result;
}

QString TextFileHandler::get_gender(QString p_char)
{
  QString f_result = read_char_ini(p_char, "gender", "Options");

  if (f_result == "")
    return "male";
  else return f_result;
}

QString TextFileHandler::get_chat(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat", "Options");

  //handling the correct order of chat is a bit complicated, we let the caller do it
  return f_result;
}

QString TextFileHandler::get_char_shouts(QString p_char)
{
  QString f_result = read_char_ini(p_char, "shouts", "Options");

  return f_result;
}

int TextFileHandler::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "Time");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}

int TextFileHandler::get_ao2_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, "%" + p_emote, "Time");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}

int TextFileHandler::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "Emotions");

  if (f_result == "")
    return 0;
  else return f_result.toInt();
}

QString TextFileHandler::get_emote_comment(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else return result_contents.at(0);
}

QString TextFileHandler::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "";
  }
  else return result_contents.at(1);
}

QString TextFileHandler::get_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  else return result_contents.at(2);
}

int TextFileHandler::get_emote_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    qDebug() << "W: misformatted char.ini: " << p_char << ", " << QString::number(p_emote);
    return 0;
  }
  else return result_contents.at(3).toInt();
}

int TextFileHandler::get_desk_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 5)
    return -1;

  QString string_result = result_contents.at(4);
  if (string_result == "")
    return -1;

  else return string_result.toInt();
}

QString TextFileHandler::get_sfx_name(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "SoundN");

  if (f_result == "")
    return "1";
  else return f_result;
}

int TextFileHandler::get_sfx_delay(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "SoundT");

  if (f_result == "")
    return 1;
  else return f_result.toInt();
}

int TextFileHandler::get_text_delay(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "TextDelay");

  if (f_result == "")
    return -1;
  else return f_result.toInt();
}

QString TextFileHandler::get_custom_realization(QString p_char)
{
  QString f_result = read_char_ini(p_char, "realization", "Options");

  if (f_result == "")
    return get_sfx("realization");
  else return f_result;
}

bool TextFileHandler::get_blank_blip()
{
    QString result = configini->value("blank_blip", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::is_discord_enabled()
{
    QString result = configini->value("discord", "true").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_enabled()
{
    QString result = configini->value("casing_enabled", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_defence_enabled()
{
    QString result = configini->value("casing_defence_enabled", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_prosecution_enabled()
{
    QString result = configini->value("casing_prosecution_enabled", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_judge_enabled()
{
    QString result = configini->value("casing_judge_enabled", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_juror_enabled()
{
    QString result = configini->value("casing_juror_enabled", "false").value<QString>();
    return result.startsWith("true");
}

bool TextFileHandler::get_casing_steno_enabled()
{
  QString result = configini->value("casing_steno_enabled", "false").value<QString>();
  return result.startsWith("true");
}

bool TextFileHandler::get_casing_cm_enabled()
{
    QString result = configini->value("casing_cm_enabled", "false").value<QString>();
    return result.startsWith("true");
}

QString TextFileHandler::get_casing_can_host_cases()
{
  QString result = configini->value("casing_can_host_cases", "Turnabout Check Your Settings").value<QString>();
  return result;
}
