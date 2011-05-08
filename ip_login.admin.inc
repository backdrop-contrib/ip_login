<?php
// $Id: ip_login.admin.inc $

/**
 * Sets up the IP Login admin page settings
 * 
 * @return $form
 *    A normal Drupal FormAPI form
 */
function ip_login_admin_settings() {
  // IP Login depends on core Profile module...
  $result = db_query("SELECT DISTINCT name FROM {profile_fields}");
  while ($field = db_fetch_object($result)) {
    $fields[$field->name] = $field->name;
  }
  if (!$fields || !variable_get('ip_login_profile_ip_field', '')) {
    drupal_set_message(
      t("You can use the !profile_module to add a text field to the user profile to store the user's ip address.",
        array('!profile_module' => l('profile module admin', 'admin/user/profile/add/textfield')))
      ); 
  }
  // show a warning if field is not hidden
  if (strlen(variable_get('ip_login_profile_ip_field', ''))) {
    $result = db_query("SELECT * FROM {profile_fields} WHERE name = '%s'", variable_get('ip_login_profile_ip_field', ''));
    $field = db_fetch_object($result);
    $secure = TRUE;
    if ($field->visibility != PROFILE_HIDDEN) { // Hidden profile field, only accessible by administrators, modules and themes
      drupal_set_message(
        t("The field %profile_field you have chosen to store IP matches is accessible by non-administators. It is strongly recommended this field's visibility is kept 'Hidden'.",
          array('%profile_field' => $field->name),
          'warning'
        )
      );
      $secure = FALSE;
    }
  }
  
  $form['ip_login_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('IP Login enabled'),
    '#description' => t('Check to enable IP Login, uncheck to disable. If unchecked no user will be able to login via IP address.'),
    '#default_value' => variable_get('ip_login_enabled', 1)
  );
  $form['ip_login_suppress_messages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Suppress IP Login messages'),
    '#description' => t('Check to prevent IP Login showing any login-related messages.'),
    '#default_value' => variable_get('ip_login_suppress_messages', 0)
  );
  $form['ip_login_profile_ip_field'] = array(
    '#type' => 'select',
    '#title' => t('Profile module IP address field'),
    '#description' => t('Select the profile field containing the user IP address.'),
    '#options' => $fields,
    '#default_value' => variable_get('ip_login_profile_ip_field', 'profile_ip')
  );
  // show extra warning if not profile field visibliliy not hidden
  if (!$secure) {
    $form['profile_field_security_warning'] = array(
      '#type' => 'markup',
      '#value' => '<div class="error">' . t("This profile field's visiblity should ideally be set to 'Hidden' for best security") . '</div>'
    );
  }
  $form['ip_login_destination'] = array(
    '#type' => 'textfield',
    '#title' => t('Destination after successful login by IP'),
    '#description' => t('<strong>Leave blank to send user back to their originally requested page</strong>, or enter the Drupal path users should be redirected to when automatically logged in. For example "user" or "&lt;front&gt;" etc.'),
    '#default_value' => variable_get('ip_login_destination', '')
  );

  $form['login_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login page link'),
    '#description' => t('A link with text from the <em>Login page link text</em> to log the user in automatically will be added to the login page unless it is blank.'),
  );
  $form['login_page']['ip_login_link_login_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Login page link text'),
    '#description' => t('Text for link on login page. Blank to remove link, defaults to "Log in automatically"'),
    '#default_value' => t(variable_get('ip_login_link_login_page', 'Log in automatically')),
  );
  $form['login_page']['ip_login_link_login_page_help'] = array(
    '#type' => 'textfield',
    '#title' => t('Login page help text'),
    '#description' => t("Help text under IP Login link on login page. Defaults to 'Your computer's IP address has been matched and validated.' and will not appear if <em>Login page link text</em> is blank."),
    '#default_value' => t(variable_get('ip_login_link_login_page_help', "Your computer's IP address has been matched and validated.")),
  );
  $form['login_page']['ip_login_link_login_page_weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 20,
    '#default_value' => variable_get('ip_login_link_login_page_weight', -10),
    '#description' => t('Optional. The position withn the login page form for this link.'),
  );
  
  $form['login_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login block link'),
    '#description' => t('A link with text from the <em>Login page link text</em> to log the user in automatically will be added to the login page unless it is blank.'),
  );
  $form['login_block']['ip_login_link_login_block'] = array(
    '#type' => 'textfield',
    '#title' => t('Login block link text'),
    '#description' => t('Text for link on login page. Use blank to remove link, defaults to "Log in automatically"'),
    '#default_value' => t(variable_get('ip_login_link_login_block', 'Log in automatically')),
    '#required' => FALSE,
  );
  $form['login_block']['ip_login_link_login_block_weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 20,
    '#default_value' => variable_get('ip_login_link_login_block_weight', -10),
    '#description' => t('Optional. The position withn the login block form for this link.'),
  );

  // allows ip check on certain pages only
  $form['active_pages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page specific IP logins'),
    '#description' => t('You can choose specific pages or paths that IP Login will or will not try to log in a user. Note that this will not affect users who have <code>can log in as another user</code> permission who have already logged out.'),
    '#collapsible' => TRUE,
  );
  $access = user_access('use PHP for block visibility');
  $pages = variable_get('ip_login_active_pages', '');
  if ($pages) {
    $check_mode = substr($pages, 0 ,1);
    $pages = substr($pages,1);
  }
  if ($check_mode == 2 && !$access) {
    $form['active_pages'] = array();
    $form['active_pages']['check_mode'] = array('#type' => 'value', '#value' => 2);
    $form['active_pages']['check_pages'] = array('#type' => 'value', '#value' => $pages);
  }
  else {
    $options = array(t('Login by IP on every page except the listed pages.'), t('Login by IP only on the listed pages.'));
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if ($access) {
      $options[] = t('login by IP if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= ' '. t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    $form['active_pages']['check_mode'] = array(
      '#type' => 'radios',
      '#title' => t('Login by IP on specific pages'),
      '#options' => $options,
      '#default_value' => $check_mode,
    );
    $form['active_pages']['check_pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => $pages,
      '#description' => $description,
    );
  }
  
  $form['#submit'][] = 'ip_login_admin_settings_submit';
  
  /* @ todo implement hook_menu_alter to add this functionality?
  $form['link_options']['ip_login_link_navigation_menu'] = array(
    '#type' => 'textfield',
    '#title' => t('Navigation menu item text'),
    '#description' => t('Text for navigation menu link. Blank to remove link, defaults to "Log in automatically"'),
    '#default_value' => variable_get('ip_login_link_navigation_menu', t('Log in automatically')),
  ); */
  return system_settings_form($form);
}


function ip_login_admin_settings_submit($form, &$form_state) {
  if ($form_state['values']['check_pages']) {
    variable_set('ip_login_active_pages', $form_state['values']['check_mode'] . $form_state['values']['check_pages']);
  }
  else {
    variable_set('ip_login_active_pages', '');
  }
}
