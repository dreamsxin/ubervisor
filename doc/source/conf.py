# -*- coding: utf-8 -*-
import sys, os
#sys.path.insert(0, os.path.abspath('.'))
#needs_sphinx = '1.0'
extensions = ['sphinx.ext.autodoc', 'sphinx.ext.doctest', 'sphinx.ext.viewcode']
templates_path = ['_templates']
source_suffix = '.txt'
master_doc = 'index'
project = u'Ubervisor'
copyright = [u'2011, Kilian Klimek', u'2011, Whitematter Labs GmbH']
version = '0.0.1'
release = '0.0.1'
exclude_patterns = []
pygments_style = 'sphinx'
html_theme = 'nature'
#html_theme_options = {}
#html_theme_path = []
#html_logo = None

html_static_path = ['_static']
htmlhelp_basename = 'Ubervisordoc'

latex_documents = [
  ('index', 'Ubervisor.tex', u'Ubervisor Documentation',
   u'Kilian Klimek', 'manual'),
]

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('command_main',   'ubervisor',        u'Ubervisor main reference', [u'Kilian Klimek'], 1),
    ('command_start',  'ubervisor-start',  u'Ubervisor start command',  [u'Kilian Klimek'], 1),
    ('command_exit',   'ubervisor-exit',   u'Ubervisor exit command',   [u'Kilian Klimek'], 1),
    ('command_kill',   'ubervisor-kill',   u'Ubervisor kill command',   [u'Kilian Klimek'], 1),
    ('command_delete', 'ubervisor-delete', u'Ubervisor delete command', [u'Kilian Klimek'], 1),
    ('command_dump',   'ubervisor-dump',   u'Ubervisor dump command',   [u'Kilian Klimek'], 1),
    ('command_get',    'ubervisor-get',    u'Ubervisor get command',    [u'Kilian Klimek'], 1),
    ('command_list',   'ubervisor-list',   u'Ubervisor list command',   [u'Kilian Klimek'], 1),
    ('command_server', 'ubervisor-server', u'Ubervisor server command', [u'Kilian Klimek'], 1),
    ('command_update', 'ubervisor-update', u'Ubervisor update command', [u'Kilian Klimek'], 1),
]

