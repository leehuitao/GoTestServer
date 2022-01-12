#include "emotion_widget.h"
#include "ui_emotion_widget.h"

#include <QLabel>
#include <QMovie>
#include <QTextCursor>

EmotionWidget::EmotionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmotionWidget)
{
    ui->setupUi(this);
}

EmotionWidget::~EmotionWidget()
{
    delete ui;
}

//添加表情
void EmotionWidget::addEmotionItem(QString fileName)
{
    // 获取当前添加到第几行第几列

    int row = m_emotionList.size() / ui->tableWidget->rowCount();
    int column = m_emotionList.size() % ui->tableWidget->rowCount();

    QTableWidgetItem* tableWidgetItem = new QTableWidgetItem;
    ui->tableWidget->setItem(row, column, tableWidgetItem);

    // 因为表情大部分为gif图片格式，所以这里用QMovie来显示
    QLabel* emotionIcon = new QLabel;
    emotionIcon->setMargin(4);
    QMovie* movie = new QMovie;
    movie->setScaledSize(QSize(24, 24));
    movie->setFileName(fileName);
    movie->start();
    emotionIcon->setMovie(movie);
    ui->tableWidget->setCellWidget(row, column, emotionIcon);
    m_emotionList.push_back(fileName);
}
//初始化表情
void EmotionWidget::initEmotion()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);

    QString path = ":/resource/emoji/%1.gif";
    for (int i = 0; i < 16; i++)
    {
        addEmotionItem(path.arg(i + 1));
    }
}

void EmotionWidget::on_tableWidget_cellClicked(int row, int column)
{
    emojiClicked(row*4+column);
}
