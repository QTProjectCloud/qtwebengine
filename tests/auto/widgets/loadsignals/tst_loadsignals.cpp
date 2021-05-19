/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include "httpserver.h"
#include "../util.h"
#include "qdebug.h"
#include "qwebenginepage.h"
#include "qwebengineprofile.h"
#include "qwebenginesettings.h"
#include "qwebengineview.h"

enum { LoadStarted, LoadSucceeded, LoadFailed };
static const QList<int> SignalsOrderOnce({ LoadStarted, LoadSucceeded});
static const QList<int> SignalsOrderTwice({ LoadStarted, LoadSucceeded, LoadStarted, LoadSucceeded });
static const QList<int> SignalsOrderOnceFailure({ LoadStarted, LoadFailed });
static const QList<int> SignalsOrderTwiceWithFailure({ LoadStarted, LoadSucceeded, LoadStarted, LoadFailed });

class TestPage : public QWebEnginePage
{
public:
    QSet<QUrl> blacklist;
    int navigationRequestCount = 0;
    QList<int> signalsOrder;
    QList<int> loadProgress;

    explicit TestPage(QObject *parent = nullptr) : TestPage(nullptr, parent) { }
    TestPage(QWebEngineProfile *profile, QObject *parent = nullptr) : QWebEnginePage(profile, parent) {
        connect(this, &QWebEnginePage::loadStarted, [this] () { signalsOrder.append(LoadStarted); });
        connect(this, &QWebEnginePage::loadProgress, [this] (int p) { loadProgress.append(p); });
        connect(this, &QWebEnginePage::loadFinished, [this] (bool r) { signalsOrder.append(r ? LoadSucceeded : LoadFailed); });
    }

    void reset()
    {
        blacklist.clear();
        navigationRequestCount = 0;
        signalsOrder.clear();
        loadProgress.clear();
    }

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType, bool) override
    {
        ++navigationRequestCount;
        return !blacklist.contains(url);
    }
};

class tst_LoadSignals : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();
    void init();

private Q_SLOTS:
    void monotonicity();
    void loadStartedAndFinishedCount_data();
    void loadStartedAndFinishedCount();
    void loadStartedAndFinishedCountClick_data();
    void loadStartedAndFinishedCountClick();
    void rejectNavigationRequest_data();
    void rejectNavigationRequest();
    void loadAfterInPageNavigation_qtbug66869();
    void fileDownload();
    void numberOfStartedAndFinishedSignalsIsSame_data();
    void numberOfStartedAndFinishedSignalsIsSame();
    void loadFinishedAfterNotFoundError_data();
    void loadFinishedAfterNotFoundError();
    void errorPageTriggered_data();
    void errorPageTriggered();

private:
    void clickLink(QPoint linkPos);

    QWebEngineProfile profile;
    TestPage page{&profile};
    QWebEngineView view;
    QSignalSpy loadStartedSpy{&page, &QWebEnginePage::loadStarted};
    QSignalSpy loadFinishedSpy{&page, &QWebEnginePage::loadFinished};
    void resetSpies() {
        loadStartedSpy.clear();
        loadFinishedSpy.clear();
    }
};

void tst_LoadSignals::initTestCase()
{
    view.setPage(&page);
    view.resize(640, 480);
    view.show();
    QVERIFY(QTest::qWaitForWindowExposed(&view));
}

void tst_LoadSignals::init()
{
    // Reset content
    if (!view.url().isEmpty()) {
        loadFinishedSpy.clear();
        view.load(QUrl("about:blank"));
        QTRY_COMPARE(loadFinishedSpy.count(), 1);
    }
    resetSpies();
    page.reset();
}

void tst_LoadSignals::clickLink(QPoint linkPos)
{
    // Simulate left-clicking on link.
    QTRY_VERIFY(view.focusProxy());
    QWidget *renderWidget = view.focusProxy();
    QTest::mouseClick(renderWidget, Qt::LeftButton, {}, linkPos);
}

/**
  * Test that we get the expected number of loadStarted and loadFinished signals
  */
void tst_LoadSignals::loadStartedAndFinishedCount_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QList<int>>("expectedSignals");
    QTest::newRow("Simple") << QUrl("qrc:///resources/page1.html") << SignalsOrderOnce;
    QTest::newRow("SimpleWithAnchor") << QUrl("qrc:///resources/page2.html#anchor") << SignalsOrderOnce;
    QTest::newRow("SamePageImmediate") << QUrl("qrc:///resources/page5.html") << SignalsOrderOnce;
    QTest::newRow("SamePageDeferred") << QUrl("qrc:///resources/page3.html") << SignalsOrderOnce;
    QTest::newRow("OtherPageImmediate") << QUrl("qrc:///resources/page6.html") << SignalsOrderOnce;
    QTest::newRow("OtherPageDeferred") << QUrl("qrc:///resources/page7.html") << SignalsOrderTwice;
    QTest::newRow("SamePageImmediateJS") << QUrl("qrc:///resources/page8.html") << SignalsOrderOnce;
}

void tst_LoadSignals::loadStartedAndFinishedCount()
{
    QFETCH(QUrl, url);
    QFETCH(QList<int>, expectedSignals);

    view.load(url);

    int expectedLoadCount = expectedSignals.size() / 2;
    QTRY_COMPARE(loadStartedSpy.size(), expectedLoadCount);
    QTRY_COMPARE(loadFinishedSpy.size(), expectedLoadCount);

    // verify no more signals is emitted by waiting for another loadStarted or loadFinished
    QTRY_LOOP_IMPL(loadStartedSpy.size() != expectedLoadCount || loadFinishedSpy.size() != expectedLoadCount, 1000, 100);

    // No further signals should have occurred within this time and expected number of signals is preserved
    QCOMPARE(loadStartedSpy.size(), expectedLoadCount);
    QCOMPARE(loadFinishedSpy.size(), expectedLoadCount);
    QCOMPARE(page.signalsOrder, expectedSignals);
}

/**
 * Load a URL, then simulate a click to load a different URL.
 */
void tst_LoadSignals::loadStartedAndFinishedCountClick_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<int>("numberOfSignals");
    QTest::newRow("SamePage") << QUrl("qrc:///resources/page2.html") << 0; // in-page navigation to anchor shouldn't emit anything
    QTest::newRow("OtherPage") << QUrl("qrc:///resources/page1.html") << 1;
}

void tst_LoadSignals::loadStartedAndFinishedCountClick()
{
    QFETCH(QUrl, url);
    QFETCH(int, numberOfSignals);

    view.load(url);
    QTRY_COMPARE(loadStartedSpy.size(), 1);
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());
    resetSpies();

    clickLink(QPoint(10, 10));
    if (numberOfSignals > 0) {
        QTRY_COMPARE(loadStartedSpy.size(), numberOfSignals);
        QTRY_COMPARE(loadFinishedSpy.size(), numberOfSignals);
        QVERIFY(loadFinishedSpy[0][0].toBool());
    }

    // verify no more signals is emitted by waiting for another loadStarted or loadFinished
    QTRY_LOOP_IMPL(loadStartedSpy.size() != numberOfSignals || loadFinishedSpy.size() != numberOfSignals, 1000, 100);

    // No further loadStarted should have occurred within this time
    QCOMPARE(loadStartedSpy.size(), numberOfSignals);
    QCOMPARE(loadFinishedSpy.size(), numberOfSignals);
    QCOMPARE(page.signalsOrder, numberOfSignals > 0 ? SignalsOrderTwice : SignalsOrderOnce);
}

void tst_LoadSignals::rejectNavigationRequest_data()
{
    QTest::addColumn<QUrl>("initialUrl");
    QTest::addColumn<QUrl>("rejectedUrl");
    QTest::addColumn<int>("expectedNavigations");
    QTest::addColumn<QList<int>>("expectedSignals");
    QTest::newRow("Simple")
            << QUrl("qrc:///resources/page1.html")
            << QUrl("qrc:///resources/page1.html")
            << 1 << SignalsOrderOnceFailure;
    QTest::newRow("SamePageImmediate")
            << QUrl("qrc:///resources/page5.html")
            << QUrl("qrc:///resources/page5.html#anchor")
            << 1 << SignalsOrderOnce;
    QTest::newRow("SamePageDeferred")
            << QUrl("qrc:///resources/page3.html")
            << QUrl("qrc:///resources/page3.html#anchor")
            << 1 << SignalsOrderOnce;
    QTest::newRow("OtherPageImmediate")
            << QUrl("qrc:///resources/page6.html")
            << QUrl("qrc:///resources/page2.html#anchor")
            << 2 << SignalsOrderOnceFailure;
    QTest::newRow("OtherPageDeferred")
            << QUrl("qrc:///resources/page7.html")
            << QUrl("qrc:///resources/page2.html#anchor")
            << 2 << SignalsOrderTwiceWithFailure;
}

/**
 * Returning false from acceptNavigationRequest means that the load
 * fails, not that the load never starts.
 *
 * See QTBUG-75185.
 */
void tst_LoadSignals::rejectNavigationRequest()
{
    QFETCH(QUrl, initialUrl);
    QFETCH(QUrl, rejectedUrl);
    QFETCH(int, expectedNavigations);
    QFETCH(QList<int>, expectedSignals);

    page.blacklist.insert(rejectedUrl);
    page.load(initialUrl);
    QTRY_COMPARE(page.navigationRequestCount, expectedNavigations);
    int expectedLoadCount = expectedSignals.size() / 2;
    QTRY_COMPARE(loadFinishedSpy.size(), expectedLoadCount);
    QCOMPARE(page.signalsOrder, expectedSignals);

    // verify no more signals is emitted by waiting for another loadStarted or loadFinished
    QTRY_LOOP_IMPL(loadStartedSpy.size() != expectedLoadCount || loadFinishedSpy.size() != expectedLoadCount, 1000, 100);

    // No further loadStarted should have occurred within this time
    QCOMPARE(loadStartedSpy.size(), expectedLoadCount);
    QCOMPARE(loadFinishedSpy.size(), expectedLoadCount);
}

/**
  * Test monotonicity of loadProgress signals
  */
void tst_LoadSignals::monotonicity()
{
    HttpServer server;
    server.setResourceDirs({ TESTS_SHARED_DATA_DIR });
    connect(&server, &HttpServer::newRequest, [] (HttpReqRep *) {
         QTest::qWait(250); // just add delay to trigger some progress for every sub resource
    });
    QVERIFY(server.start());

    view.load(server.url("/loadprogress/main.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    QVERIFY(page.loadProgress.size() >= 3);
    // first loadProgress should have 0% progress
    QCOMPARE(page.loadProgress.first(), 0);

    // every loadProgress should have more progress than the one before
    int progress = -1;
    for (int p : page.loadProgress) {
        QVERIFY(progress < p);
        progress = p;
    }

    // last loadProgress should have 100% progress
    QCOMPARE(page.loadProgress.last(), 100);
}

/**
  * Test that a second load after an in-page navigation receives its expected loadStarted and
  * loadFinished signal.
  */
void tst_LoadSignals::loadAfterInPageNavigation_qtbug66869()
{
    view.load(QUrl("qrc:///resources/page3.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // page3 does an in-page navigation after 500ms
    QTRY_COMPARE(view.url(), QUrl("qrc:///resources/page3.html#anchor"));

    // second load
    view.load(QUrl("qrc:///resources/page1.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 2);
    QVERIFY(loadFinishedSpy[0][0].toBool());
    // loadStarted and loadFinished should have been signalled
    QCOMPARE(loadStartedSpy.size(), 2);
}

void tst_LoadSignals::fileDownload()
{
    view.load(QUrl("qrc:///resources/page4.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // allow the download
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QWebEngineDownloadRequest::DownloadState downloadState = QWebEngineDownloadRequest::DownloadRequested;
    ScopedConnection sc1 =
            connect(&profile, &QWebEngineProfile::downloadRequested,
                    [&downloadState, &tempDir](QWebEngineDownloadRequest *item) {
                        connect(item, &QWebEngineDownloadRequest::stateChanged,
                                [&downloadState](QWebEngineDownloadRequest::DownloadState newState) {
                                    downloadState = newState;
                                });
                        item->setDownloadDirectory(tempDir.path());
                        item->accept();
                    });

    // trigger the download link that becomes focused on page4
    QTest::sendKeyEvent(QTest::Press, view.focusProxy(), Qt::Key_Return, QString("\r"), Qt::NoModifier);
    QTest::sendKeyEvent(QTest::Release, view.focusProxy(), Qt::Key_Return, QString("\r"), Qt::NoModifier);

    // Download must have occurred
    QTRY_COMPARE(downloadState, QWebEngineDownloadRequest::DownloadCompleted);
    QTRY_COMPARE(loadFinishedSpy.size() + loadStartedSpy.size(), 4);

    // verify no more signals is emitted by waiting for another loadStarted or loadFinished
    QTRY_LOOP_IMPL(loadStartedSpy.size() != 2 || loadFinishedSpy.size() != 2, 1000, 100);

    QCOMPARE(page.signalsOrder, SignalsOrderTwiceWithFailure);
}

void tst_LoadSignals::numberOfStartedAndFinishedSignalsIsSame_data()
{
    QTest::addColumn<bool>("imageFromServer");
    QTest::addColumn<QString>("imageResourceUrl");
    // triggers these calls in delegate internally:
    // just two ordered triples DidStartNavigation/DidFinishNavigation/DidFinishLoad
    QTest::newRow("no_image_resource") << false << "";
    // out of order: DidStartNavigation/DidFinishNavigation/DidStartNavigation/DidFailLoad/DidFinishNavigation/DidFinishLoad
    QTest::newRow("with_invalid_image") << false << "https://non.existent.locahost/image.png";
    // out of order: DidStartNavigation/DidFinishNavigation/DidStartNavigation/DidFinishLoad/DidFinishNavigation/DidFinishLoad
    QTest::newRow("with_server_image") << true << "";
}

void tst_LoadSignals::numberOfStartedAndFinishedSignalsIsSame()
{
    QFETCH(bool, imageFromServer);
    QFETCH(QString, imageResourceUrl);

    HttpServer server;
    server.setResourceDirs({ TESTS_SOURCE_DIR "/qwebengineprofile/resources" });
    QVERIFY(server.start());

    QUrl serverImage = server.url("/hedgehog.png");
    QString imageUrl(!imageFromServer && imageResourceUrl.isEmpty()
                     ? "" : (imageFromServer ? serverImage.toEncoded() : imageResourceUrl));

    auto html = "<html><head><link rel='icon' href='data:,'></head><body>"
                "%1" "<form method='GET' name='hiddenform' action='qrc:///resources/page1.html' />"
                "<script language='javascript'>document.forms[0].submit();</script>"
                "</body></html>";
    view.page()->setHtml(QString(html).arg(imageUrl.isEmpty() ? "" : "<img src='" + imageUrl + "'>"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);

    resetSpies();
    QTRY_LOOP_IMPL(loadStartedSpy.size() || loadFinishedSpy.size(), 1000, 100);
    QCOMPARE(page.signalsOrder, SignalsOrderOnce);
}

void tst_LoadSignals::loadFinishedAfterNotFoundError_data()
{
    QTest::addColumn<bool>("rfcInvalid");
    QTest::addColumn<bool>("withServer");
    QTest::addRow("rfc_invalid")  << true  << false;
    QTest::addRow("non_existent") << false << false;
    QTest::addRow("server_404")   << false << true;
}

void tst_LoadSignals::loadFinishedAfterNotFoundError()
{
    QFETCH(bool, withServer);
    QFETCH(bool, rfcInvalid);

    QScopedPointer<HttpServer> server;
    if (withServer) {
        server.reset(new HttpServer);
        QVERIFY(server->start());
    }
    view.settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
    auto url = server
        ? server->url("/not-found-page.html")
        : QUrl(rfcInvalid ? "http://some.invalid" : "http://non.existent/url");
    view.load(url);
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.count(), 1, 20000);
    QVERIFY(!loadFinishedSpy.at(0).at(0).toBool());
    QCOMPARE(toPlainTextSync(view.page()), QString());
    QCOMPARE(loadFinishedSpy.count(), 1);
    QCOMPARE(loadStartedSpy.count(), 1);
    QVERIFY(std::is_sorted(page.loadProgress.begin(), page.loadProgress.end()));
    page.loadProgress.clear();

    view.settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    url = server
        ? server->url("/another-missing-one.html")
        : QUrl(rfcInvalid ? "http://some.other.invalid" : "http://another.non.existent/url");
    view.load(url);
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.count(), 2, 20000);
    QVERIFY(!loadFinishedSpy.at(1).at(0).toBool());
    QCOMPARE(loadStartedSpy.count(), 2);

    QEXPECT_FAIL("", "No more loads (like separate load for error pages) are expected", Continue);
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.count(), 3, 1000);
    QCOMPARE(loadStartedSpy.count(), 2);
    QVERIFY(std::is_sorted(page.loadProgress.begin(), page.loadProgress.end()));
}

void tst_LoadSignals::errorPageTriggered_data()
{
    QTest::addColumn<QString>("urlPath");
    QTest::addColumn<bool>("loadSucceed");
    QTest::addColumn<bool>("triggersErrorPage");
    QTest::newRow("/content/200") << QStringLiteral("/content/200") << true << false;
    QTest::newRow("/empty/200") << QStringLiteral("/content/200") << true << false;
    QTest::newRow("/content/404") << QStringLiteral("/content/404") << false << false;
    QTest::newRow("/empty/404") << QStringLiteral("/empty/404") << false << true;
}

void tst_LoadSignals::errorPageTriggered()
{
    HttpServer server;
    connect(&server, &HttpServer::newRequest, [] (HttpReqRep *rr) {
        QList<QByteArray> parts = rr->requestPath().split('/');
        if (parts.length() != 3) {
            // For example, /favicon.ico
            rr->sendResponse(404);
            return;
        }
        bool isDocumentEmpty = (parts[1] == "empty");
        int httpStatusCode = parts[2].toInt();

        rr->setResponseHeader(QByteArrayLiteral("content-type"), QByteArrayLiteral("text/html"));
        if (!isDocumentEmpty) {
            rr->setResponseBody(QByteArrayLiteral("<html></html>"));
        }
        rr->sendResponse(httpStatusCode);
    });
    QVERIFY(server.start());

    QFETCH(QString, urlPath);
    QFETCH(bool, loadSucceed);
    QFETCH(bool, triggersErrorPage);

    view.settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    view.load(server.url(urlPath));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QCOMPARE(loadFinishedSpy[0][0].toBool(), loadSucceed);
    if (triggersErrorPage)
        QVERIFY(toPlainTextSync(view.page()).contains("HTTP ERROR 404"));
    else
        QVERIFY(toPlainTextSync(view.page()).isEmpty());
    loadFinishedSpy.clear();

    view.settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
    view.load(server.url(urlPath));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QCOMPARE(loadFinishedSpy[0][0].toBool(), loadSucceed);
    QVERIFY(toPlainTextSync(view.page()).isEmpty());
    loadFinishedSpy.clear();
}

QTEST_MAIN(tst_LoadSignals)
#include "tst_loadsignals.moc"
