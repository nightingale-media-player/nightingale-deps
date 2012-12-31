def test(mod, path, entity = None):
    import re
    # ignore anything but toolkit
    if mod not in ("netwerk", "dom", "toolkit", "security/manager",
                   "extensions/spellcheck"):
        return None

    # "report" Lorentz strings, don't "error"
    if mod == "toolkit" and path == "chrome/mozapps/plugins/plugins.dtd":
        if entity.startswith('reloadPlugin.'): return "report"
        if entity.startswith('report.'): return "report"

    return "error"
