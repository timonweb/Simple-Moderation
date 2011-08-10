SIMPLE MODERATION 7.x.-1.0
Supported Drupal version: 7.x
author: Tim Kamanin tim@timonweb.com
=============

This module creates simple moderation workflow for Drupal: it lets to enable moderation mode
for any content type which enables site editors to:
- view user submitted content subject to moderation
- delete / disapprove submitted content and send a message to the author with the
reason why the content has been rejected
- sends email notifications to site editors and content authors on the following events:
  - new content added
  - content approved
  - content disapproved
All email templates are configurable with tokens enabled
- provides administration interface powered by Views to review user submitted content

INSTALLATION
============

1) Place this module directory in your "modules" folder (this will usually be
   "sites/all/modules/"). Don't install your module in Drupal core's "modules"
   folder, since that will cause problems and is bad practice in general. If
   "sites/all/modules" doesn't exist yet, just create it.

2) Enable the Simple Moderation module in Drupal at:
   administration -> site configuration -> modules (admin/build/modules)

3) Go to content type and enable simple moderation for it