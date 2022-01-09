#ifndef EMOTION_WIDGET_H
#define EMOTION_WIDGET_H

#include <QWidget>

namespace Ui {
class EmotionWidget;
}

class EmotionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmotionWidget(QWidget *parent = nullptr);
    ~EmotionWidget();

    void initEmotion();

signals:
    void emojiClicked(int number);
private slots:
    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::EmotionWidget *ui;
    void addEmotionItem(QString fileName);

    QStringList         m_emotionList;
};

#endif // EMOTION_WIDGET_H
