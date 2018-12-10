#include "file_functions.h"

#include "aoimage.h"

AOImage::AOImage(QWidget *parent) : QLabel(parent)
{
  m_parent = parent;
}

AOImage::~AOImage()
{

}

void AOImage::set_image(QString p_image)
{
  QString theme_image_path = TextFileHandler::getInstance().get_theme_path(p_image);
  QString default_image_path = TextFileHandler::getInstance().get_default_theme_path(p_image);

  QString final_image_path;

  if (file_exists(theme_image_path))
    final_image_path = theme_image_path;
  else
    final_image_path = default_image_path;

  QPixmap f_pixmap(final_image_path);

  this->setPixmap(f_pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio));
}

void AOImage::set_image_from_path(QString p_path)
{
  QString default_path = TextFileHandler::getInstance().get_default_theme_path("chatmed.png");

  QString final_path;

  if (file_exists(p_path))
    final_path = p_path;
  else
    final_path = default_path;

  QPixmap f_pixmap(final_path);

  this->setPixmap(f_pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio));
}
