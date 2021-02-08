function checkUrl(u)
{
    if (u.includes('play.qobuz.com/album/')) {
        return u.match(/.+\/([a-zA-Z0-9]+)/)[1];
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
            chrome.tabs.create({url: 'prelude://' + url , active: true});
        }
        break;
    }
});
