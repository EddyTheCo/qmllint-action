# qmllint-action

[TOC]

This action creates PR comments with the errors found by [qmllint](https://doc.qt.io/qt-6/qtqml-tooling-qmllint.html).
The action needs `pull-requests: write` permissions, the json qmllint output files and the PR number.


## Usage

### Inputs

```
- uses: EddyTheCo/qmllint-action/@v0.1.0
  with:
    # Absolute path to the json qmllint output files
    jsondir:  
    
    # Pull request number the action will create the comment	
    pr_number: ${{ steps.getprn.outputs.prn }}

    # Absolute path of the qmllint analized repo when qmllint was runned. 
    # This is used to get relative file paths to create reviews on qml files of the github repo.
    repodir: 
```
## Examples

The [examples](examples) folder will showcase the bot when someone does a PR to this repo.
One can add complicated qml errors to the example project and when one does a PR the bot will comment the QML errors.


## Contributing

We appreciate any contribution!


You can open an issue or request a feature.
You can open a PR to the `develop` branch and the CI/CD will take care of the rest.
Make sure to acknowledge your work, and ideas when contributing.

