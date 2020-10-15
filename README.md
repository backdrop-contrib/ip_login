# IP Login

Log in automatically via their IP (v4 and v6) address ranges or wildcards instead of
having to enter a username and password - plus many other features.

## Features

* IP address matching works on:
  * Normal single IPv4s, comma separated: `123.4.5.6` or `123.4.5.6, 234.5.6.7`
  * Normal single IPv6s, leading zeroes suppressed, shortened, comma separated: `2001:db8::1:151:1, 2001:678::`
  * IPv4 ranges: `123.4.5.6-10` or `123.4-111.5.6`
  * IPv4 wildcards: `123.4.5.*` or `123.*.*.*`
  * Any combination: `127.0.0.1`, `123.4.5-66.*`, `234.4-5.6-77.*`
  * TODO: Subnet matching via CIDR notation.
* Integration with User Login page & block, with customisable text and labels
* 'Log in by IP' block with simple auto-login link for those who don't want
  to use the modified User Login block.
* Users with permission can log out and and back into another account.
* Can be set to auto-login (or not) only on certain pages, or when custom PHP
  returns TRUE.
* Optional destination address can be set when logged in by IP.
* Lists IP-enabled users on the admin page at 'Site configuration' -> 'IP
  Login'.
* Will work with external caches like Varnish when IP Login is set to
    auto-login only on specific paths.

## How it works

When a user visits any Backdrop page (or admin-chosen page), IP Login will:

  1. Check if the user is not logged in and that IP login hasn't run yet for
    this session.
  2. If so, it compare the IP address (using Backdrop's `ip_address()` function) to
    IP ranges stored in its own private table in the database.
  3. If a match is found, IP Login logs the user in programmatically as the
    account matching the IP address.
  4. If a matching IP is not found, the user can log in via normal Backdrop
    accounts if they choose to.
  5. If the user has permission to log out, they can and will be given the
    option to login back in normally as another user. Users without this
    permission are auto-logged back in IP Login. This way you can force an IP
    address to stay associated and logged into an account.

## Installation

### From scratch

  1. Enable the module from Backdrop admin -> modules
  2. Edit user accounts you want to add IP Login to.
  3. Optional: Go to the IP Login settings page to tweak and administer.
  4. Optional: Add the simple 'Log in by IP' link block to a region.

## How to test

You can test it by entering your IP address in the IP login field when
editing your user profile, then:

  1. If the site is running on your computer you can probably use the localhost
    loopback IP address of 127.0.0.1 for testing, otherwise you will need to
    find your external IP address. IP Login shows your IP address according to
    Backdrop on its admin page, but you could also use at <http://whatismyip.com>.
  2. You can tell it's working when you click the logout button and are either
    logged back in by IP Login or, if you have permission or are user 1, have
    the 'Log in automatically' option in the login form.
  3. IP Login displays a welcome message: "Welcome %name. You have been
    automatically logged into %sitename".
  4. You can also open the site in your browser when set to 'Private Browsing'
    mode (also called InPrivate, Incognito etc.) and see the welcome message
    indicating IP Login has logged you in.

## Notes & possible incompatibilities

* This module may be suitable for users with dynamic IP addresses, like those
  assigned by DHCP or from an ISP's IP blocks etc. provided that:
    a) your're ok that anyone using these IP ranges is allowed to log in with a
    given account; and
    b) the range of possible IPs for the user is known.
  If not then IP Login will not be able to work effectively in your case.

* IP Login used to be incompatible with the following modules. It had changed
  slightly since these issues were reported so it might well work now. You
  should note that any module that wipes and/or recreates the session will
  possibly have compatibility issues with IP Login:
  * Legal
  * Secure Pages Hijack Prevention

## Current Maintainers

* [Herb v/d Dool](https://github.com/herbdool)
* Seeking additional maintainers.

## Credits

* Ported to Backdrop by [Herb v/d Dool](https://github.com/herbdool/).
* Originally developed for Drupal by [Jim Kirkpatric](https://www.drupal.org/u/jim-kirkpatrick)
  and [davidwhthomas](https://www.drupal.org/u/davidwhthomas).

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
