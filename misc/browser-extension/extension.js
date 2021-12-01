function checkUrl(u)
{
    if (u.includes('qobuz.com/album/')) {
        return 'http://127.0.0.1:3333/add_qobuz/' + u.match(/.+\/([a-zA-Z0-9]+)/)[1];
    } else if (u.includes('tidal.com/album/')) {
        return 'http://127.0.0.1:3333/add_tidal/' + u.match(/.+\/([a-zA-Z0-9]+)/)[1];
    } else if (u.includes('tidal.com/browse/album/')) {
        return 'http://127.0.0.1:3333/add_tidal/' + u.match(/.+\/([a-zA-Z0-9]+)/)[1];
    } else {
        return "";
    }
}

chrome.contextMenus.removeAll(() => {
    chrome.contextMenus.create({
        id: "open_with_prelude",
        title: "Open With Prelude",
        contexts: ["selection", "link", "page"],
    });
});

chrome.contextMenus.onClicked.addListener((info, tab) => {
    switch (info.menuItemId) {
    case "open_with_prelude":
        console.log(info);
        var url = "";
        if ("selectionText" in info) {
            url = checkUrl(info.selectionText);
        } else if ("linkUrl" in info) {
            url = checkUrl(info.linkUrl);
        } else if ("pageUrl" in info) {
            url = checkUrl(info.pageUrl);
        }
        if (url !== "") {
            chrome.tabs.create({url: url , active: true});
        }
        break;
    }
});
