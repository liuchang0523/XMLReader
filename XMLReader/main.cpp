#include <QtCore/QCoreApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QStringList>

#include <opencv2/opencv.hpp>
using namespace cv;

Mat readText(const QString &str, QXmlStreamReader &reader)
{
	std::vector<double> vec;
	Mat result;
	// 如果没有读到文档结尾，而且没有出现错误
	while (!reader.atEnd()) {
		// 读取下一个记号，它返回记号的类型
		QXmlStreamReader::TokenType type = reader.readNext();
		if (type == QXmlStreamReader::StartElement) {
			qDebug() << "<" << reader.name() << ">";
			if (reader.name() == str)
			{
				type = reader.readNext();
				int row;
				if (type == QXmlStreamReader::Characters
					&& !reader.isWhitespace())
				{
					qDebug() << reader.text();
					QString text(reader.text().toUtf8());
					row = text.count("\n") - 1;
					int start = 1;
					int index = text.indexOf("\n", start);
					while (index > 0)
					{
						QString temp_row(text.mid(start, index - start));
						qDebug() << temp_row;
						int col = temp_row.count(" ") + 1;
						QStringList list = temp_row.split(" ");
						for (int i = 0; i < list.size(); ++i)
						{
							vec.push_back(list.at(i).toDouble());
						}
						start = index + 1;
						index = text.indexOf("\n", start);
					}
				}
				result = Mat(vec);
				result = result.reshape(0, row);
				result = result.clone();//深拷贝
				return result;
			}
		}
	}
	// 如果读取过程中出现错误，那么输出错误信息
	if (reader.hasError()) {
		qDebug() << "error: " << reader.errorString();
	}
	return result;
}



int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QFile file("stereoCalibrateResult.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "Error: cannot open file";
		return 1;
	}

	QXmlStreamReader reader;
	// 设置文件，这时会将流设置为初始状态
	reader.setDevice(&file);

	Mat matrix_1 = readText("matrix_1", reader);
	Mat dist_1 = readText("distortion_1", reader);
	Mat matrix_2 = readText("matrix_2", reader);
	Mat dist_2 = readText("distortion_2", reader);
	Mat R = readText("R", reader);
	Mat T = readText("T", reader);

	// 如果读取过程中出现错误，那么输出错误信息
	if (reader.hasError()) {
		qDebug() << "error: " << reader.errorString();
	}

	std::cout << R << std::endl;

	file.close();

	return a.exec();
}
