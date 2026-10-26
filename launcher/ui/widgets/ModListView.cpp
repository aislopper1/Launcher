/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ModListView.h"
#include <QDrag>
#include <QHeaderView>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>

ModListView::ModListView(QWidget* parent) : QTreeView(parent)
{
    setAllColumnsShowFocus(true);
    setExpandsOnDoubleClick(false);
    setRootIsDecorated(false);
    setSortingEnabled(true);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setHeaderHidden(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    viewport()->setAcceptDrops(true);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void ModListView::startDrag([[maybe_unused]] Qt::DropActions supportedActions)
{
    QModelIndexList indexes;
    for (const auto& index : selectedIndexes()) {
        if (index.column() == 0 && index.flags().testFlag(Qt::ItemIsDragEnabled)) {
            indexes.append(index);
        }
    }

    if (indexes.isEmpty()) {
        return;
    }

    QMimeData* mimeData = model()->mimeData(indexes);
    if (mimeData == nullptr) {
        return;
    }

    auto* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    // We do the actual move in the model's dropMimeData(). Avoid the base implementation,
    // which removes the dragged rows after a successful move.
    drag->exec(Qt::MoveAction, Qt::MoveAction);
}

void ModListView::setModel(QAbstractItemModel* model)
{
    QTreeView::setModel(model);
    auto head = header();
    head->setStretchLastSection(false);
    // HACK: this is true for the checkbox column of mod lists
    auto string = model->headerData(0, head->orientation()).toString();
    if (head->count() < 1) {
        return;
    }
    if (!string.size()) {
        head->setSectionResizeMode(0, QHeaderView::Interactive);
        head->setSectionResizeMode(1, QHeaderView::Stretch);
        for (int i = 2; i < head->count(); i++)
            head->setSectionResizeMode(i, QHeaderView::Interactive);
    } else {
        head->setSectionResizeMode(0, QHeaderView::Stretch);
        for (int i = 1; i < head->count(); i++)
            head->setSectionResizeMode(i, QHeaderView::Interactive);
    }
}

void ModListView::setResizeModes(const QList<QHeaderView::ResizeMode>& modes)
{
    auto head = header();
    for (int i = 0; i < modes.count(); i++) {
        head->setSectionResizeMode(i, modes[i]);
    }
}
