def test(mod, path, entity = None):
  import re
  # ignore anything but Firefox
  if mod not in ("netwerk", "dom", "toolkit", "security/manager",
                 "browser", "extensions/reporter", "extensions/spellcheck",
                 "other-licenses/branding/firefox"):
    return "ignore"

  if mod != "browser" and mod != "extensions/spellcheck":
    # we only have exceptions for browser and extensions/spellcheck
    return "error"
  if not entity:
    if mod == "extensions/spellcheck":
      return "ignore"
    # browser
    return (re.match(r"searchplugins\/.+\.xml", path) and
            "ignore" or "error")
            
  if mod == "extensions/spellcheck":
    # l10n ships en-US dictionary or something, do compare
    return "error"

  # just browser remains
  if path == "defines.inc":
    return (entity == "MOZ_LANGPACK_CONTRIBUTORS") and "ignore" or "error"

  # Ignore Lorentz strings, at least temporarily
  if path == 'chrome/browser/browser.properties':
    if entity.startswith('crashedpluginsMessage.'): return "report"
  if path == 'chrome/browser/preferences/advanced.dtd':
    if entity.startswith('submitCrashes'): return "report"

  if path != "chrome/browser-region/region.properties":
    # only region.properties exceptions remain, compare all others
    return "error"
  
  return ((re.match(r"browser\.search\.order\.[1-9]", entity) or
              re.match(r"browser\.contentHandlers\.types\.[0-5]", entity) or
              re.match(r"gecko\.handlerService\.schemes\.", entity) or
              re.match(r"gecko\.handlerService\.defaultHandlersVersion", entity))
          and "ignore" or "report")
