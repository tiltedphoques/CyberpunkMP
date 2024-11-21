#pragma once

struct ErrorHandler : google::protobuf::compiler::MultiFileErrorCollector
{
    void RecordError(absl::string_view filename, int line, int column, absl::string_view message) override;
};