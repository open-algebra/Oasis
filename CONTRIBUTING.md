# Code Style
We use a format based off [WebKit](https://webkit.org/code-style-guidelines/). We provide a `.clang-format` and we recommend using ClangFormat to ensure consistent formatting.
# Git Courtesies
Before making any changes, create a new branch within your fork and work on that branch. This keeps the master branch free to then fast-forward changes from upstream. When you make a pull request, pull request from that branch directly to upstream. For continued work, make a new branch from your fast-forwaded main after your changes are merged.
```mermaid
flowchart LR
subgraph matthew-mccall/Oasis
  subgraph matthew.master [master]
    matthew.A["A"] --> matthew.B["B"] --> matthew.C["C"] --> matthew.D["D"]
  end
end
subgraph your/Oasis
  direction TB
  subgraph your.master [master]
    your.B["B"]
  end
  subgraph feature
    direction LR
    your.feature.B["B"] --> your.feature.D["D"]
  end
  subgraph feature2
    direction LR
    your.feature2.B["B"] --> your.feature2.D["F"]
  end
  your.B --> your.feature.B
  your.B --> your.feature2.B
end
matthew.B --> your.B
your.feature.D --> matthew.D
```
