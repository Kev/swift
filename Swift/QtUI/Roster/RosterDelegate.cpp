#include "RosterDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QFontMetrics>
#include <QPainterPath>
#include <qdebug.h>

#include "QtTreeWidgetItem.h"

namespace Swift {

RosterDelegate::RosterDelegate() : nameFont_(QApplication::font()), statusFont_(QApplication::font()), groupFont_(QApplication::font()) {
	int statusFontSizeDrop = nameFont_.pointSize() >= 10 ? 2 : 0;
	statusFont_.setStyle(QFont::StyleItalic);
	statusFont_.setPointSize(nameFont_.pointSize() - statusFontSizeDrop);
	groupFont_.setPointSize(nameFont_.pointSize() - statusFontSizeDrop);
}
	
QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (!item || !item->isContact()) {
		return groupSizeHint(option, index);
	}
	return contactSizeHint(option, index);
}

QSize RosterDelegate::groupSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	QFontMetrics nameMetrics(groupFont_);
	return QSize(150, nameMetrics.height() + 2);
	return QStyledItemDelegate::sizeHint(option, index);
}

QSize RosterDelegate::contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	int heightByAvatar = avatarSize_ + verticalMargin_ * 2;
	QFontMetrics nameMetrics(nameFont_);
	QFontMetrics statusMetrics(statusFont_);
	int sizeByText = 2 * verticalMargin_ + nameMetrics.height() + statusMetrics.height();
	//Doesn't work, yay! FIXME: why?
	//QSize size = (option.state & QStyle::State_Selected) ? QSize(150, 80) : QSize(150, avatarSize_ + margin_ * 2);
	//qDebug() << "Returning size" << size;
	return QSize(150, sizeByText > heightByAvatar ? sizeByText : heightByAvatar);
}

void RosterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	if (item && !item->isContact()) {
		paintGroup(painter, option, index);
	} else {
		paintContact(painter, option, index);
	}
}

void RosterDelegate::paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	painter->save();		
	painter->setPen(QPen(QColor(189, 189, 189)));
	QRect region(QPoint(option.rect.left(), option.rect.top() + verticalMargin_), QSize(option.rect.width(), option.rect.height() - 2 * verticalMargin_));
	QLinearGradient fillGradient(region.topLeft(), region.bottomLeft());
	fillGradient.setColorAt(0, QColor(244, 244, 244));
	fillGradient.setColorAt(0.1, QColor(231, 231, 231));
	fillGradient.setColorAt(1, QColor(209, 209, 209));
	
	QBrush backgroundBrush = QBrush(fillGradient);
	QPainterPath fillPath;
	fillPath.addRoundedRect(region, groupCornerRadius_, groupCornerRadius_);
	QPainterPath linePath;
	linePath.addRoundedRect(region, groupCornerRadius_, groupCornerRadius_);
	painter->fillPath(fillPath, backgroundBrush);
	painter->drawPath(linePath);
	QRect textRect = region.adjusted(horizontalMargin_, 0, -1 * horizontalMargin_, 0);
	painter->setFont(groupFont_);
	painter->setPen(QPen(QColor(254, 254, 254)));
	painter->drawText(textRect.adjusted(1, 1, 0, 0), Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	painter->setPen(QPen(QColor(80, 80, 80)));
	painter->drawText(textRect, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	painter->restore();
}

void RosterDelegate::paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	//qDebug() << "painting" << index.data(Qt::DisplayRole).toString();
	painter->save();
	//QStyledItemDelegate::paint(painter, option, index);
	//initStyleOption(option, index);
	QRect fullRegion(option.rect);
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} else {
		QColor nameColor = index.data(Qt::TextColorRole).value<QColor>();
		painter->setPen(QPen(nameColor));
	}
	
	QRect presenceIconRegion(QPoint(farLeftMargin_, fullRegion.top()), QSize(presenceIconWidth_, fullRegion.height()));
	
	int calculatedAvatarSize = fullRegion.height() - 2 * verticalMargin_;
	//This overlaps the presenceIcon, so must be painted first
	QRect avatarRegion(QPoint(presenceIconRegion.right() - presenceIconWidth_ / 2, fullRegion.top() + verticalMargin_), QSize(calculatedAvatarSize, calculatedAvatarSize));
	QIcon avatar = index.data(AvatarRole).isValid() && !index.data(AvatarRole).value<QIcon>().isNull()
		? index.data(AvatarRole).value<QIcon>()
		: QIcon(":/icons/avatar.png");
	avatar.paint(painter, avatarRegion, Qt::AlignVCenter | Qt::AlignHCenter);

	//Paint the presence icon over the top of the avatar
	QIcon presenceIcon = index.data(PresenceIconRole).isValid() && !index.data(PresenceIconRole).value<QIcon>().isNull()
		? index.data(PresenceIconRole).value<QIcon>()
		: QIcon(":/icons/offline.png");
	presenceIcon.paint(painter, presenceIconRegion, Qt::AlignBottom | Qt::AlignHCenter);
	
	painter->setFont(nameFont_);
	QRect textRegion(fullRegion.adjusted(avatarRegion.right() + horizontalMargin_ * 2, 0, 0, 0));
	
	QFontMetrics nameMetrics(nameFont_);
	int nameHeight = nameMetrics.height() + verticalMargin_;
	QRect nameRegion(textRegion.adjusted(0, verticalMargin_, 0, 0));
	
	painter->drawText(nameRegion, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	
	painter->setFont(statusFont_);
	painter->setPen(QPen(QColor(160,160,160)));
	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	painter->drawText(statusTextRegion, Qt::AlignTop, index.data(StatusTextRole).toString());
	
	painter->restore();
}

const int RosterDelegate::avatarSize_ = 20;
const int RosterDelegate::presenceIconHeight_ = 16;
const int RosterDelegate::presenceIconWidth_ = 16;
const int RosterDelegate::groupCornerRadius_ = 0;
const int RosterDelegate::horizontalMargin_ = 4;
const int RosterDelegate::verticalMargin_ = 1;
const int RosterDelegate::farLeftMargin_ = 2;

}