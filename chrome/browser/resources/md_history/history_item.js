// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

Polymer({
  is: 'history-item',

  properties: {
    // The date of these history items.
    historyDate: {
      type: String,
      value: ''
    },

    readableTimestamp: {
      type: String,
      value: ''
    },

    websiteTitle: {
      type: String,
      value: ''
    },

    // Domain is the website text shown on the history-item next to the title.
    // Gives the user some idea of which history items are different pages
    // belonging to the same site, and can be used to look for more items
    // from the same site.
    websiteDomain: {
      type: String,
      value: ''
    },

    // The website url is used to define where the link should take you if
    // you click on the title, and also to define which icon the history-item
    // should display.
    websiteUrl: {
      type: String,
      value: '',
      observer: 'showIcon_'
    },

    // If the website is a bookmarked page starred is true.
    starred: {
      type: Boolean,
      value: false,
      reflectToAttribute: true
    },

    // The time in seconds of when the website was accessed.
    timestamp: {
      type: Number,
      value: 0
    },

    // Search term used to obtain this history-item.
    searchTerm: {
      type: String,
      value: '',
    },

    selected: {
      type: Boolean,
      value: false,
      notify: true
    },

    isCardStart: {
      type: Boolean,
      value: false,
      reflectToAttribute: true
    },

    isCardEnd: {
      type: Boolean,
      value: false,
      reflectToAttribute: true
    },

    hasTimeGap: {
      type: Boolean,
      value: false
    },

    numberOfItems: {
      type: Number,
      value: 0
    }
  },

  observers: [
    'setSearchedTextToBold_(websiteTitle, searchTerm)'
  ],

  /**
   * When a history-item is selected the toolbar is notified and increases
   * or decreases its count of selected items accordingly.
   * @private
   */
  onCheckboxSelected_: function() {
    this.fire('history-checkbox-select', {
      countAddition: this.$.checkbox.checked ? 1 : -1
    });
  },

  /**
   * When the url for the history-item is set, the icon associated with this
   * website is also set.
   * @private
   */
  showIcon_: function() {
    this.$.icon.style.backgroundImage =
        getFaviconImageSet(this.websiteUrl);
  },

  /**
   * Fires a custom event when the menu button is clicked. Sends the details of
   * the history item and where the menu should appear.
   */
  onMenuButtonTap_: function(e) {
    this.fire('toggle-menu', {
      target: Polymer.dom(e).localTarget,
    });

    // Stops the 'tap' event from closing the menu when it opens.
    e.stopPropagation();
  },

  /**
   * If the results shown are search results set the search term to be bold
   * where it is displayed in the history-item title.
   * @private
   */
  setSearchedTextToBold_: function() {
    var i = 0;
    var title = this.$.title;

    if (this.searchTerm == '' || this.searchTerm == null) {
      title.textContent = this.websiteTitle;
      return;
    }

    var re = new RegExp(quoteString(this.searchTerm), 'gim');
    var match;
    title.textContent = '';
    while (match = re.exec(this.websiteTitle)) {
      if (match.index > i)
        title.appendChild(document.createTextNode(
            this.websiteTitle.slice(i, match.index)));
      i = re.lastIndex;
      // Mark the highlighted text in bold.
      var b = document.createElement('b');
      b.textContent = this.websiteTitle.substring(match.index, i);
      title.appendChild(b);
    }
    if (i < this.websiteTitle.length)
      title.appendChild(document.createTextNode(this.websiteTitle.slice(i)));
  },

  selectionNotAllowed_: function() {
    return !loadTimeData.getBoolean('allowDeletingHistory');
  },

  /**
   * Generates the title for this history card.
   * @param {number} numberOfItems The number of items in the card.
   * @param {string} search The search term associated with these results.
   * @private
   */
  cardTitle_: function(numberOfItems, historyDate, search) {
    var resultId = numberOfItems == 1 ? 'searchResult' : 'searchResults';

    if (search) {
      return loadTimeData.getStringF('foundSearchResults', numberOfItems,
                                     loadTimeData.getString(resultId), search);
    } else {
      return historyDate;
    }
  }
});
