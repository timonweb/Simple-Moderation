<?php
// $Id$

/**
 * @file
 * Module Settings Interface
 */

/**
 * Menu callback -- ask for confirmation of node deletion
 */
function simod_node_form_disapprove_confirm($form, &$form_state, $node) {
  $form['#node'] = $node;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['nid'] = array('#type' => 'value', '#value' => $node->nid);
  // Textarea to enter the message to the author
  $form['disapproved-message'] = array(
    '#title' => t('Message to the author of this <i>@type</i>:', array('@type' => node_type_get_name($node))),
    '#type' => 'textarea',
    '#description' => t('Here you can enter the reason for rejection of %title, this message will be sent to the author, otherwise he will receive a default email notification.', array('%title' => $node->title)),
    '#default_value' => '',
    '#cols' => 40,
    '#rows' => 3,
    '#resizable' => FALSE,
  );
  
  return confirm_form($form,
    t('Are you sure you want to delete @type %title?', array('%title' => $node->title, '@type' => node_type_get_name($node))),
    'node/' . $node->nid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function simod_node_form_disapprove_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $node = node_load($form_state['values']['nid']);
    $simod_data['disapproved-message'] = $form_state['values']['disapproved-message'];
    $author = user_load($node->uid);
    node_delete($form_state['values']['nid']);
    watchdog('content', '@type: deleted %title.', array('@type' => $node->type, '%title' => $node->title));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => node_type_get_name($node), '%title' => $node->title)));
    // Send message to author
    drupal_mail('simod', 'disapproved', isset($node->anonymous_author['mail']) ? $node->anonymous_author['mail'] : $author->mail, user_preferred_language($author), array('node' => $node, 'simod' => $simod_data));
    // Send message to admin
    drupal_mail('simod', 'disapproved', _simod_admin_emails(), user_preferred_language($author), array('node' => $node, 'simod' => $simod_data));
  }

  $form_state['redirect'] = '<front>';
}

/**
 * Implements hook_form().
 */
function simod_admin_settings($form, &$form_state) {
  $form = array();
 
  $form['#tree'] = TRUE;
  
  $form['mail'] = array(
    '#title' => t('E-mail addresses to send new content notifications'),
    '#type' => 'textarea',
    '#description' => t('Enter addresses separated by commas'),
    '#default_value' => _simod_admin_emails(),
    '#cols' => 40,
    '#rows' => 2,
    '#resizable' => FALSE,
  );
  
  $email_templates = simod_email_templates();
  
  foreach ($email_templates AS $type => $info) {
    $form['templates'][$type] = array(
      '#type' => 'fieldset',
      '#title' => t(ucfirst($type) . ' mail template'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t($info['#description']),
    );
    $form['templates'][$type]['title'] = array(
      '#title' => t('Message title'),
      '#type' => 'textfield',
      '#default_value' => variable_get('simod_mail_' . $type . '_title', $info['#default_template']['title']),
      '#required' => TRUE,
      '#size' => 128,
    );
    $form['templates'][$type]['message'] = array(
      '#title' => t('Message Body'),
      '#type' => 'textarea',
      '#default_value' => variable_get('simod_mail_' . $type . '_message', $info['#default_template']['message']),
      '#cols' => 40,
      '#rows' => 3,
      '#resizable' => FALSE,
      '#required' => TRUE,
    );
    // Add the token help
    $form['templates'][$type]['token_help'] = array(
      '#type' => 'fieldset',
      '#title' => t('Available Tokens List'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['templates'][$type]['token_help']['content'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('node', ($type == 'disapproved') ? 'simod' : NULL),
    );
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Form Validation handler
 */
function simod_admin_settings_validate($form, &$form_state) {
  // Check if mail field is valid
  if ($form_state['values']['mail']) {
    $mails = array();
    // Get all emails out of string
    $mails = explode(',', $form_state['values']['mail']);
    foreach ($mails AS $mail) {
      if (!valid_email_address(trim($mail))) {
        form_set_error('mail', t('The e-mail address you specified is not valid.'));
        break;
      }
    }
  }
}

/**
 * Form Submit handler
 */
function simod_admin_settings_submit($form, &$form_state) {
  // Set email addresses
  variable_set('simod_admin_email', $form_state['values']['mail']);
  // Set email templates
  foreach ($form_state['values']['templates'] AS $type => $values) {
    variable_set('simod_mail_' . $type . '_title', $values['title']);
    variable_set('simod_mail_' . $type . '_message', $values['message']);
  }
  drupal_set_message('Settings has been saved');
}