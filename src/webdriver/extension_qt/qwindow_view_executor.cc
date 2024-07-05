/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "extension_qt/qwindow_view_executor.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"
#include "q_key_converter.h"
#include "qml_view_util.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtCore/QDateTime>
#ifdef OS_ANDROID
    #include <QtAndroidExtras/QtAndroidExtras>
    #include <qpa/qplatformnativeinterface.h>
    #include <jni.h>
#endif //OS_ANDROID

namespace webdriver {

QWindowViewCmdExecutor::QWindowViewCmdExecutor(Session* session, ViewId viewId)
    : ViewCmdExecutor(session, viewId) {
#if (QT_VERSION <= QT_VERSION_CHECK(6, 0, 0))
     touchDevice.setCapabilities(QTouchDevice::Velocity);
#endif
}

QWindowViewCmdExecutor::~QWindowViewCmdExecutor() {
};

QWindow* QWindowViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QWindow* pWindow = QQmlViewUtil::getQWindowView(session_, viewId);

    if (NULL == pWindow) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pWindow;
}

void QWindowViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *title = view->title().toStdString();

    session_->logger().Log(kFineLogLevel, "GetTitle - "+*title);
}

void QWindowViewCmdExecutor::GetWindowName(std::string* name, Error ** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *name = view->title().toStdString();

    session_->logger().Log(kFineLogLevel, "GetWindowName - "+*name);
}

void QWindowViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *bounds = ConvertQRectToRect(view->geometry());
}
    
void QWindowViewCmdExecutor::SetBounds(const Rect& bounds, Error** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    view->setGeometry(ConvertRectToQRect(bounds));
}

void QWindowViewCmdExecutor::Maximize(Error** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    view->showMaximized();    
}

void QWindowViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string err_msg;
    std::list<QKeyEvent> key_events;
    int modifiers = session_->get_sticky_modifiers();

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               false,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "SendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "SendKeys - cant convert keys:"+err_msg);
        return;
    }

    session_->set_sticky_modifiers(modifiers);

    std::list<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {

        bool consumed = WDEventDispatcher::getInstance()->dispatch(&(*it));

        if (!consumed)
            QGuiApplication::sendEvent(view, &(*it));
        ++it;
    }
}

void QWindowViewCmdExecutor::Close(Error** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kInfoLogLevel, "close View("+view_id_.id()+")");

    session_->RemoveView(view_id_);

    view->close();
    view->deleteLater();
}

void QWindowViewCmdExecutor::SwitchTo(Error** error) {
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->set_current_view(view_id_);

    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");
}

void QWindowViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    std::vector<ElementId> elements;
    FindElements(root_element, locator, query, &elements, error);
    if (*error == NULL && !elements.empty())
        *element = elements[0];
    else if(*error == NULL)
        *error = new Error(kNoSuchElement);
}

void QWindowViewCmdExecutor::SetOrientation(const std::string &orientation, Error **error)
{
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;

    int screen_orientation;
    if (orientation == "LANDSCAPE")
       screen_orientation = 0;
    else if (orientation == "PORTRAIT")
       screen_orientation = 1;
    else
    {
        *error = new Error(kBadRequest, "Invalid \"orientation\" parameter");
        return;
    }

#ifdef OS_ANDROID
    QPlatformNativeInterface *interface = QGuiApplication::platformNativeInterface();
    jobject jactivity = (jobject)interface->nativeResourceForIntegration("QtActivity");
    QAndroidJniObject activity(jactivity);
    activity.callMethod<void>("setRequestedOrientation", "(I)V", screen_orientation);
#else
    if (((screen_orientation == 0) && (view->height() > view->width())) || 
        ((screen_orientation == 1) && (view->height() < view->width()))) 
    {
        view->setGeometry(view->x(), view->y(), view->height(), view->width());
    }

#endif
}

void QWindowViewCmdExecutor::GetOrientation(std::string *orientation, Error **error)
{
    QWindow* view = getView(view_id_, error);
    if (NULL == view)
        return;
#ifdef OS_ANDROID
    int android_orientation;

    QPlatformNativeInterface *interface = QGuiApplication::platformNativeInterface();
    jobject jactivity = (jobject)interface->nativeResourceForIntegration("QtActivity");
    QAndroidJniObject activity(jactivity);
    android_orientation = activity.callMethod<jint>("setRequestedOrientation", "()I");

    if (android_orientation == 0)
        *orientation = "LANDSCAPE";
    else if (android_orientation == 1)
        *orientation = "PORTRAIT";
    else
    {
        *error = new Error(kUnknownError, "Unknown orientation");
        return;
    }

#else
    // This command is emulating
    if (view->height() > view->width())
        *orientation = "PORTRAIT";
    else
        *orientation = "LANDSCAPE";
#endif
}

Rect QWindowViewCmdExecutor::ConvertQRectToRect(const QRect &rect) {
    return Rect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect QWindowViewCmdExecutor::ConvertRectToQRect(const Rect &rect) {
    QRect resultRect;
    resultRect.setX(rect.x());
    resultRect.setY(rect.y());
    resultRect.setWidth(rect.width());
    resultRect.setHeight(rect.height());

    return resultRect;
}

QPoint QWindowViewCmdExecutor::ConvertPointToQPoint(const Point &p) {
    QPoint resultPoint;
    resultPoint.setX(p.x());
    resultPoint.setY(p.y());

    return resultPoint;
}

Qt::MouseButton QWindowViewCmdExecutor::ConvertMouseButtonToQtMouseButton(MouseButton button) {
    Qt::MouseButton result = Qt::NoButton;

    switch(button)
    {
        case kLeftButton: result = Qt::LeftButton; break;
        case kMiddleButton: result = Qt::MiddleButton; break;
        case kRightButton: result = Qt::RightButton; break;
        default: result = Qt::NoButton;
    }

    return result;
}

QTouchEvent::TouchPoint QWindowViewCmdExecutor::createTouchPoint(Qt::TouchPointState state, QPointF &point, QVector2D velocity)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QEventPoint::State convertedState = QEventPoint::Unknown;
    switch(state) {
        case Qt::TouchPointPressed:
            convertedState = QEventPoint::Pressed;
            break;
        case Qt::TouchPointMoved:
            convertedState = QEventPoint::Updated;
            break;
        case Qt::TouchPointStationary:
            convertedState = QEventPoint::Stationary;
            break;
        case Qt::TouchPointReleased:
            convertedState = QEventPoint::Released;
            break;
    }
    QTouchEvent::TouchPoint touchPoint(1, convertedState, point, point);

    touchPoint.velocity() = velocity;
#else
    QTouchEvent::TouchPoint touchPoint(1);
    touchPoint.setPos(point);
    touchPoint.setState(state);
    touchPoint.setPressure(1);

    touchPoint.setVelocity(velocity);
#endif
    return touchPoint;
}

QTouchEvent::TouchPoint QWindowViewCmdExecutor::createTouchPointWithId(Qt::TouchPointState state, QPointF &point, int id)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QEventPoint::State convertedState = QEventPoint::Unknown;
    switch(state) {
        case Qt::TouchPointPressed:
            convertedState = QEventPoint::Pressed;
            break;
        case Qt::TouchPointMoved:
            convertedState = QEventPoint::Updated;
            break;
        case Qt::TouchPointStationary:
            convertedState = QEventPoint::Stationary;
            break;
        case Qt::TouchPointReleased:
            convertedState = QEventPoint::Released;
            break;
    }
    QTouchEvent::TouchPoint touchPoint(id, convertedState, point, point);
#else
    QTouchEvent::TouchPoint touchPoint(id);
    touchPoint.setPos(point);
    touchPoint.setState(state);
    touchPoint.setPressure(1);
#endif

    return touchPoint;
}

QTouchEvent* QWindowViewCmdExecutor::createSimpleTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, QPointF &point, QVector2D velocity)
{
    QList<QTouchEvent::TouchPoint> points;
    Qt::TouchPointState touchPointState;
    if (touchPointStates & Qt::TouchPointPressed)
        touchPointState = Qt::TouchPointPressed;
    else if (touchPointStates & Qt::TouchPointReleased)
        touchPointState = Qt::TouchPointReleased;
    else {
        touchPointState = Qt::TouchPointMoved;
    }
    QTouchEvent::TouchPoint touchPoint = createTouchPoint(touchPointState, point, velocity);
    points.append(touchPoint);
    return createTouchEvent(eventType, touchPointStates, points);
}

QTouchEvent* QWindowViewCmdExecutor::create2PointTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, QPointF &point1, QPointF &point2)
{
    QList<QTouchEvent::TouchPoint> points;
    Qt::TouchPointState touchPointState;
    if (touchPointStates & Qt::TouchPointPressed)
        touchPointState = Qt::TouchPointPressed;
    else if (touchPointStates & Qt::TouchPointReleased)
        touchPointState = Qt::TouchPointReleased;
    else {
        touchPointState = Qt::TouchPointMoved;
    }
    QTouchEvent::TouchPoint touchPoint1 = createTouchPointWithId(touchPointState, point1, 0);
    QTouchEvent::TouchPoint touchPoint2 = createTouchPointWithId(touchPointState, point2, 1);
    points.append(touchPoint1);
    points.append(touchPoint2);
    return createTouchEvent(eventType, touchPointStates, points);
}

QTouchEvent* QWindowViewCmdExecutor::createTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, const QList<QTouchEvent::TouchPoint> &touchPoints)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QTouchEvent *touchEvent = new QTouchEvent(eventType, nullptr, Qt::NoModifier, touchPoints);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchEvent = new QTouchEvent(eventType, &touchDevice, Qt::NoModifier, touchPointStates, touchPoints);
    QDateTime current = QDateTime::currentDateTime();
    ulong timestame = current.toMSecsSinceEpoch() & (((qint64)1<<(sizeof(ulong)*8))-1);
    touchEvent->setTimestamp(timestame);
#else
    QTouchEvent *touchEvent = new QTouchEvent(eventType, QTouchEvent::TouchScreen, Qt::NoModifier, touchPointStates, touchPoints);
#endif
    return touchEvent;
}

} // namespace webdriver     
