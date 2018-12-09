#ifndef AOSFXPLAYER_H
#define AOSFXPLAYER_H

#include "bass.h"

#include <QWidget>
#include <string.h>
#include <QDebug>

class AOSfxPlayer
{
public:
  AOSfxPlayer(QWidget *parent);

  void play(QString p_sfx, QString p_char = "", QString shout = "");
  void stop();
  void set_volume(int p_volume);

private:
  QWidget *m_parent;

  int m_volume = 0;
  HSTREAM m_stream;
};

#endif // AOSFXPLAYER_H
