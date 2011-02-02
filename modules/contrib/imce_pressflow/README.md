
This is fallbacks the `file_create_url` method to drupal's default behavior, i.e. `file_create_url('')` will result in something like `sites/default/files`. The main aim is to make IMCE 2.x(which relies on `file_create_url('')`) running under pressflow core.

For the further updates consider to watch issue [#597718][].

[#597718]: https://bugs.launchpad.net/pressflow/+bug/597718
  "file.inc function file_create_url() not correct" 
