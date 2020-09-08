#include "aoscene.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "misc_functions.h"

AOScene::AOScene(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;
  preanim_timer = new QTimer(this);
  preanim_timer->setSingleShot(true);
  shout_timer = new QTimer(this);
  shout_timer->setTimerType(Qt::PreciseTimer);
  shout_timer->setSingleShot(true);
  connect(shout_timer, SIGNAL(timeout()), this, SLOT(shout_timer_done()));

  effect_timer = new QTimer(this);
  effect_timer->setTimerType(Qt::PreciseTimer);
  effect_timer->setSingleShot(true);
  connect(effect_timer, SIGNAL(timeout()), this, SLOT(effect_timer_done()));

  ticker = new QTimer(this);
  ticker->setTimerType(Qt::PreciseTimer);
  ticker->setSingleShot(false);
  connect(ticker, SIGNAL(timeout()), this, SLOT(movie_ticker()));

  //  connect(m_movie, SIGNAL(frameChanged(int)), this,
  //  SLOT(frame_change(int)));
  connect(preanim_timer, SIGNAL(timeout()), this, SLOT(preanim_done()));
}

void AOScene::load_image(image_type p_type, QStringList p_params)
{
// p_params
// backgrounds:
// .at(0) bg name
// foregrounds:
// .at(0) image name
// .at(1) misc name
// characters:
// .at(0) char name
// .at(1) emote name
// .at(2) emote prefix
// interjections:
// .at(0) image name
// .at(1) char name
// .at(2) misc name
// .at(3) delay
// interface:
// .at(0) element name
// .at(1) misc name
#ifdef DEBUG_MOVIE
  actual_time.restart();
#endif
  QList<QString> pathlist;
  is_effect = false;
  int duration = 0;
  switch (p_type) {
  case image_type::background:
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_background_path(p_params.at(0))),
        ao_app->get_image_suffix(
            ao_app->get_default_background_path(p_params.at(0)))};
    break;
  case image_type::foreground:
    pathlist = {
        ao_app->get_image_suffix(
            ao_app->get_misc_path(p_params.at(1), p_params.at(0))),
        ao_app->get_image_suffix(ao_app->get_theme_path(p_params.at(0))),
        ao_app->get_image_suffix(
            ao_app->get_default_theme_path(p_params.at(0)))};
    break;
  case image_type::character:
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_params.at(0), p_params.at(2) + p_params.at(1))), // Default path
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_params.at(0),
            p_params.at(2) + "/" +
                p_params.at(
                    1))), // Path check if it's categorized into a folder
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_params.at(0),
            p_params.at(
                1))), // Just use the non-prefixed image, animated or not
        ao_app->get_image_suffix(
            ao_app->get_theme_path("placeholder")), // Theme placeholder path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "placeholder"))}; // Default theme placeholder path
    m_char = p_params.at(0);
    m_emote = p_params.at(2) + p_params.at(1);
    break;
  case image_type::interjection:
    duration = p_params.at(3).toInt();
    play_once = true;
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_params.at(1), p_params.at(0))), // Character folder
        ao_app->get_image_suffix(
            ao_app->get_misc_path(p_params.at(2), p_params.at(0))), // Misc path
        ao_app->get_image_suffix(
            ao_app->get_theme_path(p_params.at(0))), // Theme path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            p_params.at(0))), // Default theme path
        ao_app->get_image_suffix(
            ao_app->get_theme_path("placeholder")), // Placeholder path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "placeholder")), // Default placeholder path
    };
    break;
  case image_type::effect:
    effect_duration = p_params.at(3).toInt();
    is_effect = true;
    pathlist = {
        ao_app->get_image_suffix(ao_app->get_character_path(
            p_params.at(1), "effects/" + p_params.at(0))), // Character folder
        ao_app->get_image_suffix(ao_app->get_misc_path(
            p_params.at(2), "effects/" + p_params.at(0))), // Misc path
        ao_app->get_image_suffix(
            ao_app->get_theme_path("effects/" + p_params.at(0))), // Theme path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "effects/" + p_params.at(0))), // Default theme path
        ao_app->get_image_suffix(
            ao_app->get_theme_path("placeholder")), // Placeholder path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "placeholder")), // Default placeholder path
    };
    break;
  case image_type::fuck_interface:
    pathlist = {
        ao_app->get_image_suffix(
            ao_app->get_misc_path(p_params.at(1), p_params.at(0))),
        ao_app->get_image_suffix(ao_app->get_theme_path(p_params.at(0))),
        ao_app->get_image_suffix(
            ao_app->get_default_theme_path(p_params.at(0)))};
    break;
  }
  QString image_path;
  for (QString path : pathlist) {
    if (file_exists(path)) {
      image_path = path;
      break;
    }
  }

  this->clear();
  ticker->stop();
  preanim_timer->stop();
  movie_frames.clear();
  movie_delays.clear();
  movie_effects.clear();

  if (!file_exists(image_path))
    return;

  m_reader->setFileName(image_path);
  if (m_reader->loopCount() == 0)
    play_once = true;
  else
    play_once = false;
  QPixmap f_pixmap = this->get_pixmap(m_reader->read());
  int f_delay = m_reader->nextImageDelay();

  frame = 0;
  max_frames = m_reader->imageCount();

  this->set_frame(f_pixmap);
  this->show();
  if (max_frames > 1) {
    movie_frames.append(f_pixmap);
    movie_delays.append(f_delay);
  }

  if (network_strings.size() > 0) // our FX overwritten by networked ones
    this->load_network_effects();
  else // Use default ini FX
    this->load_effects();

  if (duration > 0)
    shout_timer->start(duration);
  if (effect_duration > 0)
    effect_timer->start(effect_duration);
  play();
#ifdef DEBUG_MOVIE
  qDebug() << max_frames << "Setting image to " << image_path
           << "Time taken to process image:" << actual_time.elapsed();

  actual_time.restart();
#endif
}

void AOScene::set_play_once(bool p_play_once) { play_once = p_play_once; }

void AOScene::load_effects()
{
  movie_effects.clear();
  movie_effects.resize(max_frames);
  for (int e_frame = 0; e_frame < max_frames; ++e_frame) {
    QString effect = ao_app->get_screenshake_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("shake");
    }

    effect = ao_app->get_flash_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("flash");
    }

    effect = ao_app->get_sfx_frame(m_char, m_emote, e_frame);
    if (effect != "") {
      movie_effects[e_frame].append("sfx^" + effect);
    }
  }
}

void AOScene::load_network_effects()
{
  movie_effects.clear();
  movie_effects.resize(max_frames);
  // Order is important!!!
  QStringList effects_list = {"shake", "flash", "sfx^"};

  // Determines which list is smaller - effects_list or network_strings - and
  // uses it as basis for the loop. This way, incomplete network_strings would
  // still be parsed, and excess/unaccounted for networked information is
  // omitted.
  int effects_size = qMin(effects_list.size(), network_strings.size());

  for (int i = 0; i < effects_size; ++i) {
    QString netstring = network_strings.at(i);
    QStringList emote_splits = netstring.split("^");
    foreach (QString emote, emote_splits) {
      QStringList parsed = emote.split("|");
      if (parsed.size() <= 0 || parsed.at(0) != m_emote)
        continue;
      foreach (QString frame_data, parsed) {
        QStringList frame_split = frame_data.split("=");
        if (frame_split.size() <=
            1) // We might still be hanging at the emote itself (entry 0).
          continue;
        int f_frame = frame_split.at(0).toInt();
        if (f_frame >= max_frames) {
          qDebug() << "Warning: out of bounds" << effects_list[i] << "frame"
                   << f_frame << "out of" << max_frames << "for" << m_char
                   << m_emote;
          continue;
        }
        QString f_data = frame_split.at(1);
        if (f_data != "") {
          QString effect = effects_list[i];
          if (effect == "sfx^") // Currently the only frame result that feeds us
                                // data, let's yank it in.
            effect += f_data;
          qDebug() << effect << f_data << "frame" << f_frame << "for" << m_char
                   << m_emote;
          movie_effects[f_frame].append(effect);
        }
      }
    }
  }
}

void AOScene::play()
{
  play_frame_effect(frame);
  if (max_frames <= 1) {
    if (play_once)
      ticker->start(60);
  }
  else
    ticker->start(this->get_frame_delay(movie_delays[frame]));
}

void AOScene::play_pre(QString p_char, QString p_emote, int duration)
{
  QList<QString> params = {p_char, p_emote, ""};
  load_image(image_type::character, params);
  // As much as I'd like to screw around with [Time] durations modifying the
  // animation speed, I don't think I can reliably do that, not without looping
  // through all frames in the image at least - which causes lag. So for now it
  // simply ends the preanimation early instead.
  play_once = true;
  if (duration >
      0) // It's -1 if there's no definition in [Time] for it. In which case, it
         // will let the animation run out in full. Duration 0 does the same.
    preanim_timer->start(duration *
                         time_mod); // This timer will not fire if the animation
                                    // finishes earlier than that
  play();
}

void AOScene::play_talking(QString p_char, QString p_emote)
{
  play_once = false;
  QList<QString> params = {p_char, p_emote, "(b)"};
  load_image(image_type::character, params);
}

void AOScene::play_idle(QString p_char, QString p_emote)
{
  play_once = false;
  QList<QString> params = {p_char, p_emote, "(a)"};
  load_image(image_type::character, params);
}

void AOScene::play_frame_effect(int frame)
{
  if (frame < max_frames) {
    foreach (QString effect, movie_effects[frame]) {
      if (effect == "shake") {
        shake();
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play shake on frame" << frame;
#endif
      }

      if (effect == "flash") {
        flash();
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play flash on frame" << frame;
#endif
      }

      if (effect.startsWith("sfx^")) {
        QString sfx = effect.section("^", 1);
        play_sfx(sfx);
#ifdef DEBUG_MOVIE
        qDebug() << "Attempting to play sfx" << sfx << "on frame" << frame;
#endif
      }
    }
  }
}

void AOScene::stop()
{
  // for all intents and purposes, stopping is the same as hiding. at no point
  // do we want a frozen gif to display
  ticker->stop();
  preanim_timer->stop();
  shout_timer->stop();
  this->hide();
}

QPixmap AOScene::get_pixmap(QImage image)
{
  QPixmap f_pixmap;
  if (m_flipped)
    f_pixmap = QPixmap::fromImage(image.mirrored(true, false));
  else
    f_pixmap = QPixmap::fromImage(image);
  //    auto aspect_ratio = Qt::KeepAspectRatio;
  auto transform_mode = Qt::FastTransformation;
  if (f_pixmap.height() > f_h) // We are downscaling, use anti-aliasing.
    transform_mode = Qt::SmoothTransformation;
  if (is_effect)
    f_pixmap = f_pixmap.scaled(f_w, f_h, Qt::IgnoreAspectRatio, transform_mode);
  else
    f_pixmap = f_pixmap.scaledToHeight(f_h, transform_mode);
  this->resize(f_pixmap.size());

  return f_pixmap;
}

void AOScene::set_frame(QPixmap f_pixmap)
{
  this->setPixmap(f_pixmap);
  QLabel::move(
      x + (f_w - f_pixmap.width()) / 2,
      y + (f_h - f_pixmap.height())); // Always center horizontally, always put
                                      // at the bottom vertically
}

void AOScene::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  f_w = w;
  f_h = h;
  this->resize(f_size);
}

int AOScene::get_frame_delay(int delay)
{
  return static_cast<int>(double(delay) * double(speed / 100));
}

void AOScene::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}

void AOScene::movie_ticker()
{
  ++frame;
  if (frame >= max_frames) {
    if (play_once) {
      preanim_done();
      return;
    }
    else
      frame = 0;
  }
  //  qint64 difference = elapsed - movie_delays[frame];
  if (frame >= movie_frames.size()) {
    m_reader->jumpToImage(frame);
    movie_frames.resize(frame + 1);
    movie_frames[frame] = this->get_pixmap(m_reader->read());
    movie_delays.resize(frame + 1);
    movie_delays[frame] = m_reader->nextImageDelay();
  }

#ifdef DEBUG_MOVIE
  qDebug() << frame << movie_delays[frame]
           << "actual time taken from last frame:" << actual_time.restart();
#endif

  this->set_frame(movie_frames[frame]);
  play_frame_effect(frame);
  ticker->setInterval(this->get_frame_delay(movie_delays[frame]));
}

void AOScene::preanim_done()
{
  ticker->stop();
  preanim_timer->stop();
  done();
}

void AOScene::shout_timer_done()
{
  this->stop();
  // signal connected to courtroom object, let it figure out what to do
  done();
}

void AOScene::effect_timer_done()
{
  this->stop();
  done();
}
